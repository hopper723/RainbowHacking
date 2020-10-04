//
// Created by Hiu Man Yeung on 13/8/2020.
//

#ifndef RAINBOWHACKING_RAINBOWHACKING_H
#define RAINBOWHACKING_RAINBOWHACKING_H

#include "RainbowTable.h"
#include <sys/time.h>
#include <string>

class RainbowHacking {

public:
    /**
     *
     * @param isSilent: false if commands should display results when invoked, true otherwise.
     */
    RainbowHacking();

    ~RainbowHacking();

    /**
     * Execute commands
     * @param action: command name caught from user input.
     */
    void doAction(std::string const &action);

private:

    /***************** Atrributes *****************/
    /* Rainbow table */
    RainbowTable* _rain;

    /* Static pointer to _rain. Used so that static method handleSignalCTRLC
    can free memory when user interrupts the execution. */
    static RainbowTable** _rainInstance;

    /***************** Methods *****************/
    /**
     * Prints the commands available to the user.
     */
    static void printInstructions() ;

    /**
     * Computes the time since t0, and returns it as seconds.
     * @param t0: Start time.
     * @return The time elapsed in seconds.
     */
    static double computeTime(struct timeval const &t0) ;

    double crackHash(std::string const &hash, bool &hasFound) const;

    /**
     * Cracks the hash associated to a password.
     * @param pwd: Password to hash, and then to crack.
     * @param hasFound: Placeholder to indicate whether the operation succeed.
     * @return The time the operation took.
     */
    double crackWord(std::string const &pwd, bool &hasFound) const;

    /**
     * Creates a new table, using the user inputted arguments.
     * Returns the time the operation took.
     */
    double newTable();

    /**
     * Loads a table from a file.
     * @param fileName: Path of the file to read in.
     * @return the time the operation took.
     */
    double loadTable(std::string const &filePath);

    /**
     * 
     * @return
     */
    double  extendTable();

    /**
     * Generates a file containing valid random passwords.
     * @param n: Number of passwords to generate.
     * @param fileName: Name of the file to write to.
     * @return The time the operation took.
     */
    double generatePwdFile(int n, std::string const &filePath);

    /**
     * Reads a list of passwords from a file, and tries to crack them.
     * @param fileName: Name of the file to write to.
     * @return The success percentage.
     */
    double testPwdFile(std::string const &filePath);

    /**
     * Handles the CTRL-C (interruption) signal.
     * @param signal
     */
    static void handleSignalCTRLC(int signal);
};

#endif //RAINBOWHACKING_RAINBOWHACKING_H
