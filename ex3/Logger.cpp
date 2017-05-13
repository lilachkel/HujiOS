//
// Created by jenia90 on 5/11/17.
//
#include "Logger.h"

Logger::Logger(std::string filename, bool debug) : _isDebugMode(debug)
{
    _logFile.open(filename, std::ios::out | std::ios::app);
    if (!_logFile.is_open())
    {
        exit(EXIT_FAILURE);
    }
}

Logger::~Logger()
{
    _logFile.close();
}

void Logger::Log(const std::string msg)
{
    if(_isDebugMode)
        std::cout << msg << std::endl;
    _logFile << msg << std::endl;
}
