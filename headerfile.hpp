#ifndef HEADERFILE_HPP
#define HEADERFILE_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/poll.h>
#include <string.h>
#include <map>

#define MAX_CLIENTS 100


class Server
{
    private:
    int port;
    int socket_fd;
    std::string password;
    struct sockaddr_in serverAddress;
    // struct sockaddr_in clientAddress;
    std::map<int, std::string> clients;
    struct pollfd fds[MAX_CLIENTS];


    public:
    Server(int port, std::string password);
    ~Server();
    int create_socket();
    int bind_socket();
    int listen_socket();
    void accept_socket();

    int get_port();
    int get_socket_fd();
    std::string get_password();
    void handleNewConnection(struct pollfd* fds, int& fds_num);
    void handleClientMessage(std::map<int, std::string>& clientMap, std::vector<std::string>& Channels,
                                 struct pollfd* fds, int& fds_num, int clientIndex);
    void handleClientDisconnection(std::map<int, std::string>& clientMap, struct pollfd* fds, int& fds_num, int clientIndex);
    void handleClientData(std::map<int, std::string>& clientMap, std::vector<std::string>& Channels,
                              struct pollfd* fds, int clientIndex, const char* buffer);
    void handleNewClient(std::map<int, std::string>& clientMap, struct pollfd* fds, int clientIndex, const char* buffer);
    void handleNewClientPassword(std::map<int, std::string>& clientMap, struct pollfd* fds, int clientIndex,
                                     const std::string& clientInfo);
    void handleExistingClient(std::vector<std::string>& Channels, const char* buffer);
    void handleChannelCreation(const std::string& clientInfo, std::vector<std::string>& Channels, std::string& channel, size_t pos);
    void handleClientInformation(const std::string& clientInfo) ;
    



};

void handle_client(int client_fd);

#endif