//
// Created by jenia90 on 6/20/17.
//

#ifndef PROJECT_NETWORKHANDLER_H
#define PROJECT_NETWORKHANDLER_H

#include <vector>
#include <sys/socket.h>
#include <sstream>
#include <tuple>
#include <regex>
#include "definitions.h"

/**
 * Reads data from a given file descriptor
 * @param fd file descriptor to read from
 * @return string message
 */
std::string ReadData(int fd);

/**
 * Sends message to the given file descriptor
 * @param fd destination file descriptor
 * @param message string message to send
 * @return -1 in case of an error; non-negative value otherwise.
 */
int SendData(int fd, std::string message);

/**
 * Readies a message to be sent.
 * @param message string to send
 * @return readied string.
 */
std::string readyForSend(std::string message);

/**
 * Splits the given data string using a pre-defined delimeter.
 * @param data string to parse
 * @return list of strings
 */
std::tuple<std::string, std::string, std::string> ParseData(std::string data);

#endif //PROJECT_NETWORKHANDLER_H
