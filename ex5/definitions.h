#ifndef PROJECT_DEFINITIONS_H
#define PROJECT_DEFINITIONS_H

#include <stdlib.h>
#include <string>

/**
 * Error message definitions
 */
#define WRONG_SERVER_USAGE "Usage: whatsappServer portNum"
#define WRONT_CLIENT_USAGE "Usage: whatsappClient clientName serverAddress portNum"
#define INVALID_CMD "ERROR: Invalid input.\n"
#define SYS_CALL_ERROR(func, errnum) "ERROR: " + std::string(func) + " " + std::string(errnum) + ".\n"

/**
 * Server output messages
 */
#define CREATE_GRP_SUCCESS(client, group) std::string(client) + ": Group " + std::string(group) + " was created "\
                                            "successfully.\n"
#define CREATE_GRP_FAILURE(client, group) std::string(client) + ": ERROR: failed to create group " + std::string\
                                            (group) + ".\n"
#define SEND_SUCCESS(sender, message, name) std::string(sender) + ": " + std::string(message) + " was sent "\
                                            "successfully to " + std::string(name) + ".\n"
#define SEND_FAILURE(sender, message, name) std::string(sender) + ": ERROR: failed to send " + std::string(message) +\
                                            " to " + std::string(name) ".\n"
#define WHO_REQUEST(client) std::string(client) + ": Requests the currently connected client names.\n"
#define EXIT_REQUEST(client) std::string(client) + ": Unregisterd successfully.\n"

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
#define CREATE_GROUP_CMD "create_group"
#define SEND_CMD "send"
#define WHO_CMD "who"
#define EXIT_CMD "exit"

/**
 * Constraints
 */
#define MAX_CLIENT_NAME 30
#define MAX_CLIENTS 30
#define MAX_MESSAGE_LENGTH 256

#endif //PROJECT_DEFINITIONS_H
