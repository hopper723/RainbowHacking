//
// Created by Hiu Man Yeung on 9/8/2020.
//

#ifndef RAINBOWHACKING_TABLEBUILDER_HPP
#define RAINBOWHACKING_TABLEBUILDER_HPP

#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include "HashMethod.hpp"

class Chain;  // Defined below the definition of Table
class Table;  // Structure storing hash-password pairs

/**
 * Table builder
 */
class TableBuilder {

private:
    Table* tableToBuild;

public:

    TableBuilder();

    explicit TableBuilder(unsigned int nChains, Table *tableToBuild = nullptr);

    ~TableBuilder();

    TableBuilder* init(unsigned int nChains);

    /**
     *
     * @param chain
     * @return
     */
    TableBuilder* insert(Chain *chain);

    /**
     *
     * @param pwd
     * @param hash
     * @return
     */
    TableBuilder* insert(std::string const &pwd, unsigned char const *hash);

    /**
     * Clear the table.
     */
    TableBuilder* clear();

    /**
     * Build the table
     * @return
     */
    Table* build();

};

class Table {

private:
    std::vector<Chain*> *table;

    /**
     *
     */
     void clear();

public:
    explicit Table(unsigned int nChains);

    ~Table();

    unsigned int size() const;

    /**
     *
     * @param hash
     * @return
     */
    std::vector<std::string> findPassword(unsigned char const *hash) const;


    /**
     * Prints the content of the table to stream.
     * @param stream: Output stream to write to.
     * @return The output stream
     */
    std::ostream& printTo(std::ostream &stream) const;

    friend class TableBuilder;
};

class Chain {

private:
    std::string _pwd;
    unsigned char _hash[HASH_SIZE]{};

public:

    /**
     * Constructor
     * @param pwd
     * @param hash
     */
    Chain(std::string pwd, unsigned char const *hash) : _pwd(std::move(pwd)) {
        memcpy(_hash, hash, HASH_SIZE);
    }

    /**
     * Password getter
     * @return
     */
    std::string getPwd() const {
        return _pwd;
    }

    /**
     *
     * @param hash
     */
    void getHash(unsigned char *hash) const {
        memcpy(hash, _hash, HASH_SIZE);
    }

    std::string getHashStr() const {
        return MD5Hash::convertHexString(_hash);
    }

    int compare(Chain const *chain) const {
        unsigned char hash[HASH_SIZE];
        chain->getHash(hash);

        return compare(hash);
    }

    int compare(unsigned char const *hash) const {

        int i = 0;

        while (i < HASH_SIZE && hash[i] == _hash[i]) {
            ++i;
        }

        return i == HASH_SIZE ? 0 : _hash[i] - hash[i];
    }
};

#endif //RAINBOWHACKING_TABLEBUILDER_HPP
