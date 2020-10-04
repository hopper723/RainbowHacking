//
// Created by Hiu Man Yeung on 9/8/2020.
//

#include "TableBuilder.hpp"
#include <algorithm>


/** TableBuilder implementation **/

TableBuilder::TableBuilder() {
    tableToBuild = nullptr;
}

TableBuilder::TableBuilder(unsigned int nChains, Table *tableToBuild) {
    this->tableToBuild = tableToBuild;

    if (!tableToBuild) {
        init(nChains);
    } else {
        nChains += this->tableToBuild->size();
        this->tableToBuild->table->reserve(nChains);
    }
}

TableBuilder::~TableBuilder() {
    delete tableToBuild;
}

TableBuilder* TableBuilder::init(unsigned int nChains) {
    clear();
    tableToBuild = new Table(nChains);

    return this;
}

TableBuilder* TableBuilder::insert(Chain *chain) {

    tableToBuild->table->push_back(chain);

    return this;
}

TableBuilder* TableBuilder::insert(std::string const &pwd, unsigned char const *hash) {

    auto *chain = new Chain(pwd, hash);

    return insert(chain);
}

Table* TableBuilder::build() {

    std::sort(tableToBuild->table->begin(), tableToBuild->table->end(),
              [](Chain *a, Chain *b) { return (a->compare(b) < 0); }
              );

    Table *completeTable = tableToBuild;
    tableToBuild = nullptr;

    return completeTable;
}

TableBuilder* TableBuilder::clear() {
    delete tableToBuild;
    return this;
}


/** Table implementation **/

Table::Table(unsigned int nChains) {
    table = new std::vector<Chain*>();
    table->reserve(nChains);
}

Table::~Table() {
    clear();
    delete table;
}

void Table::clear() {
    for(auto chain: *table)
        delete chain;
}

unsigned int Table::size() const {
    return table->size();
}

std::ostream& Table::printTo(std::ostream& stream) const {
    // unsigned char hash[HASH_SIZE];

    // For every hash-password pair, print it to the stream.
    for(auto chain: *table) {
        // chain->getHash(hash);
        stream << chain->getPwd() << " " << chain->getHashStr() << std::endl;
    }

    return stream;
}

std::vector<std::string> Table::findPassword(unsigned char const *hash) const {

    auto low_comp = [](Chain *it, unsigned char const *val) { return (it->compare(val) < 0); };
    auto up_comp = [](unsigned char const *val, Chain *it) { return (0 < it->compare(val)); };

    auto lo = std::lower_bound(table->begin(), table->end(), hash, low_comp);
    auto hi = std::upper_bound(table->begin(), table->end(), hash, up_comp);

    std::vector<std::string> passwords;

    unsigned int size = std::distance(lo, hi);

    if (size > 0) {
        passwords.reserve(size);

        for (auto it = lo; it != hi; ++it)
            passwords.push_back((*it)->getPwd());
    }

    return passwords;
}