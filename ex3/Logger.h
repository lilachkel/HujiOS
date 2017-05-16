#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <ostream>
#include <iostream>
#include <fstream>


class Logger
{
private:
    std::fstream _logFile;
    bool _isDebugMode;
public:
    /**
     * Filename constructor. Creates a logger from a given filename
     * @param filename file name of the log file
     * @param debug outputs to console as well as to the file if true.
     */
    Logger(std::string filename = ".MapReduceFrameworkLog", bool debug = false);

    Logger(const Logger &rhs) = delete;

    Logger(Logger &&rhs);

    Logger &operator=(Logger &&other);

    /**
     * Dtor. Closes the file stream.
     */
    ~Logger();

    /**
     * Writes the give message to the log file and to the console if _isDebug set to true.
     * @param msg Messages string to output.
     */
    void Log(const std::string msg, bool isErr = false);
};


#endif //PROJECT_LOGGER_H
