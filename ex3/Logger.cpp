#include <vector>
#include <ctime>
#include <iomanip>
#include "Logger.h"

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

std::string Logger::GetTimeString()
{
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);

    return str;
}

void Logger::Log(const std::string arg, bool isErr, DataType dataType)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::string msg;

    if (isErr)
    {
        std::cerr << "error in: " << arg << std::endl;
    }

    else
    {
        switch (dataType)
        {
            case ThreadInit:
                msg = ("Thread " + arg + " created [" + GetTimeString() + "]");
                break;
            case ThreadDeath:
                msg = ("Thread " + arg + " terminated [" + GetTimeString() + "]");
                break;
            case General:
                msg = arg;
                break;
        }
        if (_isDebugMode)
            std::cout << msg << std::endl;
        _logFile << msg << std::endl;
    }
}

template <typename ...Args>
void Debug(const std::string method, const std::string location, Args ... args)
{
    std::cout << "Method: " << method;
    std::cout << " | Location: " << location;
    std::cout << "Args: " << std::endl;
    for (const auto a : {args...})
    {
        std::cout << " | " << a << " " << std::endl;
    }
    std::cout << std::endl;
}
