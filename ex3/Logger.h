//
// Created by jenia90 on 5/11/17.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <ostream>
#include <iostream>
#include <fstream>


class Logger
{
private:
    std::ofstream _logFile;
    bool _isDebugMode;
public:
    /**
     * Filename constructor. Creates a logger from a given filename
     * @param filename file name of the log file
     * @param debug outputs to console as well as to the file if true.
     */
    Logger(std::string filename, bool debug = false);

    /**
     * Dtor. Closes the file stream.
     */
    ~Logger();

    /**
     * Writes the give message to the log file and to the console if _isDebug set to true.
     * @param msg Messages string to output.
     */
    void Log(std::string msg, bool isErr = false) const;
};


#endif //PROJECT_LOGGER_H
