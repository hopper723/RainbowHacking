//
// Created by Hiu Man Yeung on 7/8/2020.
//

#ifndef RAINBOWHACKING_HASHMETHOD_HPP
#define RAINBOWHACKING_HASHMETHOD_HPP

#include "openssl/md5.h"
#include <cstring>
#include <string>
#include <utility>

#define HASH_SIZE 16

/**
 * Hashing method interface
 */
class HashMethod {

protected:
    std::string _name;
    /**
     * Constructor
     * @param name: Name of the method
     */
    explicit HashMethod(std::string name) : _name(std::move(name)) {};

public:

    virtual ~HashMethod() = default;;

    /**
     *
     * @param pwd
     * @param hash
     */
    virtual void hash(std::string const &pwd, unsigned char *hash) const = 0;

    /**
     *
     * @return the name of the hash method
     */
    inline std::string name() {
        return _name;
    }
};

/**
 * MD5 hashing method
 */

class MD5Hash : public HashMethod {

public:
    /**
     * Constructor
     */
    MD5Hash() : HashMethod("md5") {};

    ~MD5Hash() override = default;

    void hash(std::string const &pwd, unsigned char *hash) const override {

        auto d = reinterpret_cast<const unsigned char *>(pwd.c_str());

        MD5(d, pwd.size(), hash);
    }

    /**
     * Convert char pointer to hex string
     * @param hash
     * @return
     */
    static std::string convertHexString(unsigned char const *hash) {
        std::string hex = "0123456789ABCDEF";
        std::string hashStr;

        for (int i = 0; i < HASH_SIZE; ++i) {
            hashStr += hex[hash[i] >> 4u];
            hashStr += hex[hash[i] & 15u];
        }

        return hashStr;
    }

    /**
     * Convert hex string to char array
     * @param text
     * @param hash
     */
    static void hexConvert(char const *text, unsigned char *hash) {

        // WARNING: Current implementation specific to MD5.
        // Transform the hash (string) into a byte array.
        // Since every byte is exressed with 2 chars, read it 2 by 2,
        // convert it to an int and store it into the bytes array.

        char buff[] = "00";

        for (int i = 0; i < HASH_SIZE; ++i) {
            strncpy(buff, text + 2 * i, 2);
            hash[i] = strtoul(buff, nullptr, 16);
        }
    }
};

#endif //RAINBOWHACKING_HASHMETHOD_HPP
