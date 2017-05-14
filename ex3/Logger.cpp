#include "Logger.h"

//
// Created by jenia90 on 5/11/17.
//
Logger::Logger(Logger &&rhs) : _logFile(std::move(rhs._logFile)), _isDebugMode(rhs._isDebugMode)
{
    rhs._logFile.close();
}

Logger::Logger(std::string filename, bool debug) : _isDebugMode(debug)
{
    _logFile.open(filename, std::ios::out | std::ios::app);
    if (!_logFile.is_open())
    {
        std::cerr << "Unable to access log file." << std::endl;
        exit(EXIT_FAILURE);
    }
}

Logger::~Logger()
{
    _logFile.close();
}

Logger &Logger::operator=(Logger &&other)
{
    _logFile = std::move(other._logFile);
    _isDebugMode = other._isDebugMode;
    other._logFile.close();

    return *this;
}

void Logger::Log(const std::string msg, bool isErr)
{
    if (isErr)
    {
        std::cerr << msg << std::endl;
    }
    else
    {
        if (_isDebugMode)
            std::cout << msg << std::endl;
        _logFile << msg << std::endl;
    }
}
