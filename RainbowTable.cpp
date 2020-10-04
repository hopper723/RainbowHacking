//
// Created by Hiu Man Yeung on 11/8/2020.
//

//#ifdef DEBUG
//#define DEBUG_STDOUT(x) (std::out << (x))
//#else
//#define DEBUG_STDOUT(x) do {} while(0)
//#endif

#include "RainbowTable.h"
#include "TableBuilder.hpp"
#include <random>
#include <omp.h>
#include <iostream>
#include <cstring>

RainbowTable::RainbowTable(std::string const &filePath) {
    this->initFromFile(filePath);
}

RainbowTable::RainbowTable(unsigned int chainLen, unsigned int nChains, std::string const &domain,
                           unsigned int pwdLen, HashMethod* hashMethod)
{
    this->chainLen = chainLen;
    this->domain = domain;
    this->pwdLen = pwdLen;
    this->hashMethod = hashMethod;
//    replace(chars, "a-z", LETTERSLOWER);
//    replace(chars, "A-Z", LETTERSUPPER);
//    replace(chars, "0-9", DIGITS);

    this->initTable(nChains);
}

RainbowTable::~RainbowTable() {
    delete hashMethod;
    delete table;
}

void RainbowTable::generateChains(unsigned int nChains, Table *rainbowTable) {

    const int nThreads = omp_get_max_threads();

    omp_set_num_threads(nThreads);

    std::vector<Chain*> v_arr[nThreads];

    // Will crash if nChains < 55
    const unsigned int chunkSize = (nChains + nThreads - 1) / nThreads;

    // Parallelize the generation.
    // Every thread will generates <nChains / # of threads> chains.
    #pragma omp parallel default(none) shared(nChains, chunkSize, v_arr)
    {
        std::string pwd;
        unsigned char hash[HASH_SIZE];

        int threadNum = omp_get_thread_num(); // Get thread number

        long start = threadNum * chunkSize;
        long end = start + chunkSize < nChains ? start + chunkSize : nChains;

        v_arr[threadNum].reserve(end - start);

        for (long i = start; i < end; ++i) {
            // Generate a new password
            pwd = randomPassword();
            // Generate a chain, and retrieve its last hash.
            createChain(pwd, hash);

            // Add the pair password - hash to a temporary vector.
            auto *chain = new Chain(pwd, hash);
            v_arr[threadNum].push_back(chain);
        }
    }

    TableBuilder tableBuilder(nChains, rainbowTable);

    // Add the pairs in the temporary vector to the table.
    for (int i = 0; i < nThreads; ++i) {
        while (!v_arr[i].empty()) {
            tableBuilder.insert(v_arr[i].back());
            v_arr[i].pop_back();
        }
    }

    this->table = tableBuilder.build();
}

void RainbowTable::initTable(unsigned int nChains) {

    std::cout << "Initializing table" << std::endl;

    generateChains(nChains);
}

void RainbowTable::extendTable(unsigned int nChains) {

    std::cout << "Extending table" << std::endl;

    generateChains(nChains, table);
}

std::string RainbowTable::randomPassword() const {
    std::random_device rd;  // Seed
    std::mt19937 mt(rd());  // random number generator
    std::uniform_int_distribution<> dist(0, domain.size()-1);

    // Generates a new password.
    std::string pwd;
    // Get <pwdLength> random characters from the covered characters, and
    // concatenate& them.
    for (int i=0; i < pwdLen; ++i) {
        pwd += domain[dist(mt)];
    }
    return pwd;
}

void RainbowTable::initFromFile(std::string const& filePath) {
    std::ifstream in(filePath.c_str());

    if (in) {
        // Read the parameters from the file.
        std::string hashMethodName;

        in >> this->chainLen;   // Length of the chains.
        std::cout << "chainLen: " << chainLen << std::endl;

        unsigned int nChains;
        in >> nChains;    // Number of chains
        std::cout << "nChains: " << nChains << std::endl;

        in >> this->domain;             // Available chars
        std::cout << "domain: " << domain << std::endl;

        in >> this->pwdLen;	    // Length of the passwords
        std::cout << "pwdLen: " << pwdLen << std::endl;

        in >> hashMethodName;	// Name of the hashing method
        if (hashMethodName == "md5") {
            this->hashMethod = new MD5Hash();
        }
        std::cout << "hashMethod: " << hashMethodName << std::endl;

        TableBuilder tableBuilder(nChains);

        // Read the chains.
        std::string pwd, hashStr;
        unsigned char hash[HASH_SIZE];

        while(in >> pwd >> hashStr) {
            MD5Hash::hexConvert(hashStr.c_str(), hash);
            tableBuilder.insert(pwd, hash);
        }

        this->table = tableBuilder.build();

        std::cout << "Initialized from file." << std::endl;

    } else {
        std::cerr << "Could not read from file \"" << filePath << "\"." << std::endl;
    }
}

void RainbowTable::writeToFile(std::string const &filePath) const {
    std::ofstream out(filePath.c_str());

    if (out) {
        // Write the parameters of the table.
        std::string dstr(domain);

        out << chainLen << " "  // Length of the chains.
            << table->size() << " "   // Number of chains
            << dstr << " "	    // Available chars
            << pwdLen << " "    // Length of the passwords
            << hashMethod->name() << std::endl; // Name of the hashing method.
        // Write the chains.
        table->printTo(out);

        std::cout << "Wrote to file." << std::endl;

    } else {
        std::cerr << "Could not write to file \"" << filePath << "\"." << std::endl;
    }

    out.close();
}

std::string RainbowTable::reduce(unsigned char const *hash, unsigned int k) const {
    std::string pwd;
    unsigned int index;
    // WARNING : Current implementation specific to MD5.

    for (int i = 0; i < pwdLen; ++i) {
        // Get the value of the i-nd byte of the hash. The value column is
        // added so as to same inputs at different columns will generate a
        // different reduced password. This will avoid too many repetitions
        // in the table.
        index = hash[(i + k) % HASH_SIZE] + k;
        // Get the corresponding character and concatenate it to the password.
        pwd += domain[index % domain.size()];
    }

    return pwd;
}

void RainbowTable::createChain(std::string pwd, unsigned char *hash) const {
    // Hash and reduce the starting password <columns> times.
    for (long i = 0; i < chainLen; ++i) {
        this->hashPassword(pwd, hash);
        pwd = this->reduce(hash, i);
    }
}


void RainbowTable::getEndHash(unsigned char *endHash, unsigned char const *hash, unsigned int k) const {
    std::string pwd;
    memcpy(endHash, hash, HASH_SIZE);

    // Hash and reduce the starting password <columns-starCol> times.
    for (long i = k; i < chainLen-1; ++i) {
        pwd = reduce(endHash, i);
        hashPassword(pwd, endHash);
    }
}

bool equal(unsigned char const *hash1, unsigned char const *hash2) {

    for (int i = 0; i < HASH_SIZE; ++i)
        if (hash1[i] != hash2[i])
            return false;

    return true;
}

std::string RainbowTable::findHashInChain(std::string pwd, unsigned char const *targetHash) const {
    // int i = 0;
    unsigned char hash[HASH_SIZE];

    for (long i = 0; i < chainLen; ++i) {
        hashPassword(pwd, hash);
        if (equal(hash, targetHash))
            return pwd;
        pwd = reduce(hash, i);
    }

    // Hash and reduce the password until the end of the chain has been reached,
    // or startHash has been found.

    return "";
}

void RainbowTable::hashPassword(std::string const &pwd, unsigned char *hash) const {
    // Hashes a word using the table's hashing method.
    this->hashMethod->hash(pwd, hash);
}

std::string RainbowTable::crackHash(unsigned char const *targetHash) const {

    std::string result;

    const int nThreads = omp_get_max_threads();

    omp_set_num_threads(nThreads);

    const unsigned int chunkSize = (chainLen + nThreads - 1) / nThreads;

    #pragma omp parallel default(none) shared(result, targetHash, chunkSize) // Parallelize the cracking. Every thread will
    // try cracking for < columns / nb of threads > different columns.
    {
        int threadNum = omp_get_thread_num(); // Get thread number

        unsigned int start = threadNum * chunkSize;
        unsigned int end = start + chunkSize < chainLen ? start + chunkSize : chainLen;

        std::string pwd;
        unsigned char endHash[HASH_SIZE];
        std::vector<std::string> pwdCandidates;

        for (long i = end - 1; i >= start && result.empty(); --i) {
            // Compute the final hash, when starting at column <col>.
            getEndHash(endHash, targetHash, i);

            // Find the start passwords corresponding to the hash (possibly 0, 1 or more).
            pwdCandidates = table->findPassword(endHash);

            for (auto &pwdCandidate : pwdCandidates) {
                // For every start password, try to find if the hash is contained in it.
                pwd = findHashInChain(pwdCandidate, targetHash);
                if (!pwd.empty()) {
                    // If the hash has been found, store the corresponding
                    // password, causing the loop to stop.
                    result = pwd;
                }
            }
        }
    }
    return result;
}

std::string RainbowTable::crackPassword(std::string const &password) const {
    // Hashes a password, then tries to crack it.
    unsigned char hash[HASH_SIZE];
    hashPassword(password, hash);

    return crackHash(hash);
}