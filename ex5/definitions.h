#ifndef PROJECT_DEFINITIONS_H
#define PROJECT_DEFINITIONS_H

#include <stdlib.h>
#include <string>

/**
 * Error message definitions
 */
#define WRONG_SERVER_USAGE "Usage: whatsappServer portNum"
#define WRONG_CLIENT_USAGE "Usage: whatsappClient clientName serverAddress portNum"
#define INVALID_CMD "ERROR: Invalid input.\n"
#define SYS_CALL_ERROR(func, errnum) "ERROR: " + std::string(func) + " " + std::string(errnum) + ".\n"

/**
 * Server output messages
 */
#define CREATE_GRP_SUCCESS(client, group) std::string(client) + ": Group " + std::string(group) + " was created "\
                                            "successfully."
#define CREATE_GRP_FAILURE(client, group) std::string(client) + ": ERROR: failed to create group " + std::string\
                                            (group) + "."
#define SEND_SUCCESS(sender, message, name) std::string(sender) + ": " + std::string(message) + " was sent "\
                                            "successfully to " + std::string(name) + "."
#define SEND_FAILURE(sender, message, name) std::string(sender) + ": ERROR: failed to send"\
                                            " " + std::string(message)+ " to " + std::string(name) + "."
#define WHO_REQUEST(client) std::string(client) + ": Requests the currently connected client names.\n"
#define EXIT_REQUEST(client) std::string(client) + ": Unregisterd successfully."

/**
 * Client output messages
 */
#define CREATE_GRP_SUCCESS_CLI(group) "Group " + std::string(group) + " was created successfully.\n"
#define CREATE_GRP_FAILURE_CLI(group) "ERROR: failed to create group " + std::string(group) ".\n"
#define SEND_SUCCESS_CLI "Sent successfully.\n"
#define SEND_FAILURE_CLI "ERROR: failed to send.\n"
#define CLIENT_WHO_REPLY(clients) clients.join(',') + ".\n"
#define CLIENT_WHO_FAIL "ERROR: failed to receive list of connected clients.\n"
#define CLIENT_EXIT "Unregistered successfully.\n"

/**
 * Command strings definitions
 */
const std::string CREATE_GROUP_CMD = "create_group", SEND_CMD = "send",
        WHO_CMD = "who", EXIT_CMD = "exit", CON_SUCCESS = "consucc", INVALID_USERNAME = "invuname";

/**
 * Constraints
 */
#define MAX_CLIENT_NAME 30
#define MAX_CLIENTS 30
#define MAX_MESSAGE_LENGTH 259 + sizeof(int)

/**
 * Constants
 */
#define MESSAGE_START 'S'
#define MESSAGE_LENGTH_END '#'
#define MESSAGE_END '\n'
#define GROUP_UNAME_DELIM ','


#endif //PROJECT_DEFINITIONS_H
