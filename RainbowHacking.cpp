//
// Created by Hiu Man Yeung on 13/8/2020.
//

#include "RainbowHacking.h"
#include <iostream>
#include <iomanip>
#include <csignal>
#include <cstdlib>
#include <vector>

using namespace std;

RainbowTable** RainbowHacking::_rainInstance = nullptr;

RainbowHacking::RainbowHacking() {
    this->_rain = nullptr;
    RainbowHacking::_rainInstance = &this->_rain;
    signal(SIGINT, RainbowHacking::handleSignalCTRLC);
}

RainbowHacking::~RainbowHacking() {
    delete _rain;
}

void RainbowHacking::printInstructions() {
    cout << "help -- Displays this menu." << endl;
    cout << "new [chainLen] [nChains] [domain] [pwdlen] [hashMethod] --" << endl
         << "\tCreates a new rainbow table with [nChains] chains of length [chainLen]" << endl
         << "\tpasswords of length [pwdLen] with domain [domain], and using hash function [hashMethod]" << endl
         << "\t('md5' for md5 hash)." << endl;
    cout << "crackH [hash] -- Tries to find the password with [hash]." << endl;
    cout << "crackW [password] -- Tries to find the password with the hash of [password]." << endl;
    cout << "save [filePath] -- Saves a rainbow table to [filePath]." << endl;
    cout << "load [filePath] -- Load a rainbow table from [filePath]." << endl;
    cout << "genPwd [n] [filePath] -- Generates [n] random valid passwords and writes them to [filePath]." << endl;
    cout << "testPwd [filePath] -- Reads a list of passwords from [filePath], and tries to crack them." << endl;
    cout << "quit -- Quits the program." << endl;
}

double RainbowHacking::computeTime(struct timeval const &t0) {
    struct timeval t1{};
    gettimeofday(&t1, nullptr);
    double dTime0 = t0.tv_sec + (t0.tv_usec / 1000000.0);
    double dTime1 = t1.tv_sec + (t1.tv_usec / 1000000.0);
    return dTime1 - dTime0;
}

double RainbowHacking::crackHash(string const &hashStr, bool &hasFound) const {

    unsigned char hash[HASH_SIZE];
    MD5Hash::hexConvert(hashStr.c_str(), hash);

    struct timeval t{};
    gettimeofday(&t, nullptr);

    string res = _rain->crackHash(hash);

    double time = computeTime(t);

    cout << "'" << hashStr << "' --> ";

    hasFound = !res.empty();

    if (hasFound) {
        cout << "'" << res << "'";
    } else {
        cout << "Not found...";
    }

    cout << " (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::crackWord(std::string const &pwd, bool &hasFound) const {
    struct timeval t{};
    gettimeofday(&t, nullptr);

    string res = _rain->crackPassword(pwd);

    double time = computeTime(t);

    unsigned char hash[HASH_SIZE];
    _rain->hashPassword(pwd, hash);

    cout << "'" << pwd << "' --> '" << MD5Hash::convertHexString(hash) << "' --> ";

    hasFound = !res.empty();

    if (hasFound) {
        cout << "'" << res << "'";
    } else {
        cout << "Not found...";
    }

    cout << " (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::newTable() {

    delete _rain;

    int chainLen, nChains, pwdLen;
    string ll(LETTERSLOWER);
    string lu(LETTERSUPPER);
    string d(DIGITS);

    string chars = ll + lu + d;
    HashMethod *hashMethod = new MD5Hash();
    cout << "Enter the length of chains: " << endl;
    cin >> chainLen;
    cout << "Enter the number of chains: " << endl;
    cin >> nChains;
//    cout << "Enter allowed chars: " << endl;
//    cin >> chars;
    cout << "Enter the length of password: " << endl;
    cin >> pwdLen;

    cout << "Creating new rainbow table" << endl;

    struct timeval t{};
    gettimeofday(&t, nullptr);

    _rain = new RainbowTable(chainLen, nChains, chars, pwdLen, hashMethod);

    double time = computeTime(t);
    cout << "Table generated (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::loadTable(std::string const &filePath) {

    delete _rain;

    struct timeval t{};
    gettimeofday(&t, nullptr);

    _rain = new RainbowTable(filePath);

    double time = computeTime(t);
    cout << "Table loaded (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::extendTable() {

    unsigned int nChains;

    cout << "Enter the number of chains: " << endl;
    cin >> nChains;

    struct timeval t{};
    gettimeofday(&t, nullptr);

    _rain->extendTable(nChains);

    double time = computeTime(t);
    cout << "Table extended (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::generatePwdFile(int n, std::string const &filePath) {

    struct timeval t{};
    gettimeofday(&t, nullptr);

    ofstream out(filePath.c_str());

    if (!out) {
        cerr << "Could not write to file <" << filePath << ">." << endl;

        return 0.0;
    }

    for (int i = 0; i < n; ++i) {
        out << _rain->randomPassword() << endl;
    }

    double time = computeTime(t);
    out.close();
    cout << "Password file generated (" << setprecision(4) << time << " seconds)" << endl;

    return time;
}

double RainbowHacking::testPwdFile(std::string const &filePath) {

    vector<string> passwords;
    int success = 0;
    double time = 0.0;

    ifstream in(filePath.c_str());

    if (!in) {
        cerr << "Could not read from file <" << filePath << ">." << endl;
        return time;
    }

    int i = 0;
    string pwd;

    while (in >> pwd) {
        passwords.push_back(pwd);
        ++i;
    }

    in.close();

    cout << "Total " << i << " passwords." << endl;

    /* Testing passwords */

    i = 0;
    bool hasFound;

    for (auto &password: passwords) {
        if (++i % 100 == 0) {
            cout << i << endl;
        }

        cout << i << ": ";
        time += crackWord(password, hasFound);

        success += hasFound;
        hasFound = false;
    }

    cout << success << " / " << passwords.size() << " : "
         << (success*100.0) / passwords.size() << "% ("
         << setprecision(4) << time << " seconds, "
         << time / passwords.size() << " s / pwd)" << endl;

    return time;
}

void RainbowHacking::doAction(const string& action) {
    string param1;
    string filePath;
    unsigned int nChains;
    bool res;
    int n;

    if (action == "help") { /* Print the help menu. */
        printInstructions();
    }
    else if (action == "new") { /* Create a new table. */
        newTable();
    }
    else if (action == "load") { /* Load a table from a file. */
        cout << "Enter the path" << endl;
        cout << ">>> ";
        cin >> param1;	// File name
        loadTable(param1);
    }
    else if (_rain == nullptr && action != "quit") {
        /* If the table has not yet been initialized, interrupt. */
        cout << "***You need to create or load a table first." << endl;
    }
    else if (action == "crackH") {  /* Crack a hash. */
        cout << "Enter a hash" << endl;
        cout << ">>> ";
        cin >> param1;	// Hash to crack.
        crackHash(param1, res);
    }
    else if (action == "crackW") { /* Crack a password. */
        cout << "Enter a password" << endl;
        cout << ">>> ";
        cin >> param1;	// Word to hash, and then to crack.
        crackWord(param1, res);
    }
    else if (action == "save") { /* Save the current table to a file. */
        cout << "Enter the path" << endl;
        cout << ">>> ";
        cin >> param1;	// File name
        _rain->writeToFile(param1);
    }
    else if (action == "addChain") {
        extendTable();
    }
    else if (action == "genPwd") { /* Generate a password file. */
        cout << "Enter a number" << endl;
        cout << ">>> ";
        cin >> n; // Number of passwords
        cout << "Enter the path" << endl;
        cout << ">>> ";
        cin >> param1; // File name
        generatePwdFile(n, param1);
    }
    else if (action == "testPwd") { /* Test passwords from a file. */
        cout << "Enter the path" << endl;
        cout << ">>> ";
        cin >> param1; // File name
        testPwdFile(param1);
    }
    else if (action != "quit") {
        /* Invalid command. */
        cout << action << " is not a valid command." << endl;
        // Flush buffer
        cin.clear();
        fflush(stdin);
    }
}

void RainbowHacking::handleSignalCTRLC(int signal)
{
    delete *(RainbowHacking::_rainInstance);
    cout << "Received signal: " << signal << endl;
    exit(EXIT_SUCCESS);
}

int main() {
    RainbowHacking test;
    string action;
    cout << "Enter 'help' to learn the commands." << endl;
    cout << "You should first create or load a new rainbow table." << endl;

    while (action != "quit") {
        cout << ">>> ";
        cin >> action;
        test.doAction(action);
    }
    return 0;
}