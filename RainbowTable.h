//
// Created by Hiu Man Yeung on 9/8/2020.
//

#ifndef RAINBOWHACKING_RAINBOWTABLE_H
#define RAINBOWHACKING_RAINBOWTABLE_H

#include <vector>
#include <string>
#include "HashMethod.hpp"
#include "TableBuilder.hpp"

#define LETTERSLOWER "abcdefghijklmnopqrstuvwxyz"
#define LETTERSUPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGITS "0123456789"

class RainbowTable {

private:
    unsigned int chainLen{};  /* Length each chain */
    // unsigned int nChains;
    std::string domain;           /* Array of characters to check for. */
    unsigned int pwdLen{};    /* Size of the passwords */
    Table *table{};            /* Table containing all the rows (hash + password) */
    HashMethod *hashMethod{}; /* Hashing function */

    /**
     * Initialize table.
     */
    void initTable(unsigned int nChains);

    void generateChains(unsigned int nChains, Table *rainbowTable = nullptr);

    /**
     *
     * @param hash
     * @param column
     * @return
     */
    std::string reduce(unsigned char const *hash, unsigned int column) const;

    /**
     *
     * @param pwd
     * @param hash
     */
    void createChain(std::string pwd, unsigned char *hash) const;

    /**
     *
     * @param hash
     * @param k
     */
    void getEndHash(unsigned char *endHash, unsigned char const *hash, unsigned int k) const;

    /**
     * Finds a hash in a chain.
     * @param startPwd: Start password of the chain.
     * @param startHash: Hash to find.
     * @return The password associated to the hash if it is found, "" otherwise.
     */
    std::string findHashInChain(std::string pwd, unsigned char const *targetHash) const;

public:
    /**
     * Creates a new table, which will be loaded from a file.
     * @param filePath: The path of the file to read from.
     */
    explicit RainbowTable(std::string const &filePath);

     /**
      *
      * @param chainLen: The length of each chain.
      * @param nChains: Number of chains
      * @param domain: All the available characters
      * @param pwdLen: The length of password
      * @param hashMethod: The hashing method
      */
    RainbowTable(unsigned int chainLen, unsigned int nChains,
                 std::string const &domain, unsigned int pwdLen, HashMethod* hashMethod);

    /**
     * Destructor
     */
    ~RainbowTable();

    void extendTable(unsigned int nChains);

    /**
      * Generates and returns a new correct password.
      */
    std::string randomPassword() const;

    /**
     * Initialize a table from a file.
     * @param fileName: The path of the file to read from.
     */
    void initFromFile(std::string const &filePath);

    /**
     * Write the table to a file.
     * @param filePath: The path of the file to write to.
     */
    void writeToFile(std::string const &filePath) const;

    /**
     *
     * @param pwd
     * @param hash
     */
    void hashPassword(std::string const &pwd, unsigned char *hash) const;

    /**
     *
     * @param startHash: Hash to crack.
     * @return: A password if found else "".
     */
    std::string crackHash(unsigned char const *targetHash) const;

    /**
     * Hashes a password and tries to crack it.
     * @param word: Password to hash, and then to crack.
     * @return: Word if the password is found, "" otherwise.
     */
    std::string crackPassword(std::string const &password) const;
};

#endif //RAINBOWHACKING_RAINBOWTABLE_H
