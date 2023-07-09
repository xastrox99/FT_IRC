#include "headerfile.hpp"

Server::Server(int port, std::string password)
{
    this->port = port;
    this->password = password;
    // std::cout << "from here "<<this->password << std::endl;
}

int Server::get_port()
{
    return (this->port);
}

int Server::get_socket_fd()
{
    return (this->socket_fd);
}

std::string Server::get_password()
{
    return (this->password);
}

Server::~Server()
{
    close(this->socket_fd);
}

int Server::create_socket()
{
    if((this->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout << "Failed to create socket" << std::endl;
        return -1;
    }
    int reuse = 1;
    if(setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) == -1)
    {
        std::cout << "Failed to set socket options" << std::endl;
        close(this->socket_fd);
        return -1;
    }
    return (0);
}

int Server::bind_socket()
{
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(this->socket_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cout << "Failed to bind socket" << std::endl;
        close(this->socket_fd);
        return -1;
    } 
    return (0);
}

int Server::listen_socket()
{
    if(listen(this->socket_fd, MAX_CLIENTS) == -1)
    {
        std::cout << "Failed to listen for connections" << std::endl;
        close(this->socket_fd);
        return -1;
    }
    std::cout << "Server started and listening on port " << this->port << std::endl;
    return (0);
}


void removeNewline(std::string& str) {
    if (!str.empty() && str[str.length() - 1] == '\n') {
        str.erase(str.length() - 1);
    }
}



void Server::handleNewConnection(std::map<int, std::string>& clientMap, struct pollfd* fds, int& fds_num) {
    int socket;
    size_t i = sizeof(serverAddress);
    
    if ((socket = accept(this->socket_fd, (struct sockaddr*)&serverAddress, (socklen_t*)&i)) == -1) {
        std::cout << "Failed to accept connection" << std::endl;
        close(this->socket_fd);
        return;
    }
    
    if (fds_num == MAX_CLIENTS) {
        std::cout << "Maximum number of clients reached" << std::endl;
        close(socket);
    } else {
        fds[fds_num].fd = socket;
        fds[fds_num].events = POLLIN;
        fds_num++;
        
        std::cout << "Client connected" << std::endl;
        
        // Perform authentication for the new connection
        if (performAuthentication(clientMap, fds, fds_num - 1)) {
            std::cout << "Client authenticated" << std::endl;
        } else {
            std::cout << "Authentication failed. Closing connection." << std::endl;
            close(socket);
            fds_num--;
        }
    }
}

bool Server::performAuthentication(std::map<int, std::string>& clientMap, struct pollfd* fds, int clientIndex) {
    char buffer[1024];
    memset(buffer, 0, 1024);
    int bytes = recv(fds[clientIndex].fd, buffer, 1024, 0);
    
    if (bytes <= 0) {
        std::cout << "Failed to receive client authentication data" << std::endl;
        return false;
    }
    
    std::string clientInfo(buffer);
    
    if (clientInfo.find("PASS ") == 0) {
        std::string pass = clientInfo.substr(5);
        removeNewline(pass);
        
        if (pass != password) {
            std::cout << "Incorrect password. Closing connection." << std::endl;
            return false;
        }
        
        // Add client to the map with an empty username and nickname
        clientMap[fds[clientIndex].fd] = "";
    } else {
        std::cout << "Invalid password format. Closing connection." << std::endl;
        return false;
    }
    
    return true;
}

void Server::handleClientMessage(std::map<int, std::string>& clientMap, std::vector<std::string>& Channels,
                                 struct pollfd* fds, int& fds_num, int clientIndex) 
                                 {
    char buffer[1024];
    memset(buffer, 0, 1024);
    int bytes = recv(fds[clientIndex].fd, buffer, 1024, 0);
    
    if (bytes <= 0) 
    {
        handleClientDisconnection(clientMap, fds, fds_num, clientIndex);
    } else {
        std::cout << "Received " << bytes << " bytes" << std::endl;
        std::cout << "from client: " << buffer << std::endl;
        handleClientData(clientMap, Channels, fds, clientIndex, buffer);
    }
}

void Server::handleClientDisconnection(std::map<int, std::string>& clientMap, struct pollfd* fds, int& fds_num, int clientIndex) {
    std::cout << "Client disconnected" << std::endl;
    close(fds[clientIndex].fd);
    fds[clientIndex].fd = -1;

    // Remove client from the map
    clientMap.erase(fds[clientIndex].fd);

    // Shift remaining clients in the fds array
    for (int i = clientIndex; i < fds_num - 1; ++i) {
        fds[i] = fds[i + 1];
    }
    --fds_num;
}

void Server::handleClientData(std::map<int, std::string>& clientMap, std::vector<std::string>& Channels,
                              struct pollfd* fds, int clientIndex, const char* buffer) {
    if (clientMap.find(fds[clientIndex].fd) == clientMap.end()) {
        handleNewConnection(clientMap, fds, clientIndex);
    } 
    else 
    {
        std::string clientInfo(buffer);
        std::string channel;
        size_t pos;

        pos = clientInfo.find("JOIN #");
        if (pos != std::string::npos) {
            pos += 6; // Length of "JOIN #"
            size_t endPos = clientInfo.find(" ", pos);
            if (endPos != std::string::npos) {
                channel = clientInfo.substr(pos, endPos - pos);

                // Create a new Channel object
                Channel newChannel(channel);
                channels.push_back(newChannel);

                std::cout << "Channel created: " << channel << std::endl;
            }
        } 
        else 
        {
            handleExistingClient(Channels, buffer);
        }
    }
}



void Server::handleNewClientPassword(std::map<int, std::string>& clientMap, struct pollfd* fds, int clientIndex,
                                     const std::string& clientInfo) {
    std::string pass = clientInfo.substr(5);
    removeNewline(pass);

    if (pass != password) {
        std::cout << "Incorrect password. Closing connection." << std::endl;
        close(fds[clientIndex].fd);
    } else {
        std::cout << "Client authenticated" << std::endl;
        clientMap[fds[clientIndex].fd] = "";
    }
}

void Server::handleExistingClient(std::vector<std::string>& Channels, const char* buffer) {
    std::string clientInfo(buffer);
    std::string channel;
    size_t pos;

    pos = clientInfo.find("JOIN #");
    if (pos != std::string::npos) {
        pos += 1;
        handleChannelCreation(clientInfo, Channels, channel, pos);
    } else {
        handleClientInformation(clientInfo);
    }
}

void Server::handleChannelCreation(const std::string& clientInfo, std::vector<std::string>& Channels, std::string& channel, size_t pos) {
    std::vector<std::string>::const_iterator it;
    for (it = Channels.begin(); it != Channels.end(); ++it) {
        if (*it == clientInfo.substr(pos)) {
            std::cout << "Channel already exists" << std::endl;
            return;
        }
    }
    std::cout << "Channel created" << std::endl;
    Channels.push_back(clientInfo.substr(pos));
    channel = clientInfo.substr(pos);
    std::vector<std::string>::const_iterator iter;
    for (iter = Channels.begin(); iter != Channels.end(); ++iter) {
        std::cout << *iter << std::endl;
    }
}

void Server::handleClientInformation(const std::string& clientInfo) 
{
    std::string username, nickname, password, channel;
    size_t pos;

    pos = clientInfo.find("USERNAME ");
    if (pos != std::string::npos) {
        pos += 9; // Length of "USERNAME "
        size_t endPos = clientInfo.find(" ", pos);
        if (endPos != std::string::npos) {
            username = clientInfo.substr(pos, endPos - pos);
            pos = endPos + 1;
        }
    }

    pos = clientInfo.find("NICKNAME ", pos);
    if (pos != std::string::npos) {
        pos += 9; // Length of "NICKNAME "
        size_t endPos = clientInfo.find(" ", pos);
        if (endPos != std::string::npos) {
            nickname = clientInfo.substr(pos, endPos - pos);
            pos = endPos + 1;
        }
    }

    pos = clientInfo.find("PASSWORD ", pos);
    if (pos != std::string::npos) {
        pos += 9; // Length of "PASSWORD "
        size_t endPos = clientInfo.find(" ", pos);
        if (endPos != std::string::npos) {
            password = clientInfo.substr(pos, endPos - pos);
        } else {
            password = clientInfo.substr(pos); // Password extends until the end of the string
        }
    }
    size_t newlinePos = password.find("\n");
    if (newlinePos != std::string::npos) {
        password.erase(newlinePos);
    }

    // Verify client information and password
    if (username.empty() || nickname.empty() || password.empty()) {
        std::cout << "Invalid client information. Closing connection." << std::endl;
        
        // Close the connection or take appropriate action
        return;
    }

    if (strcmp(password.c_str(), this->password.c_str())) {
        std::cout << "Incorrect password. Closing connection." << std::endl;
        // std::cout << password << std::endl;
        // Close the connection or take appropriate action
        return;
    }
}

void Server::accept_socket(void) {
    int fds_num;
    // int socket;
    size_t i;
    std::map<int, std::string> clientMap; // Map to store client information
    std::vector<std::string> Channels;

    i = sizeof(serverAddress);
    memset(fds, 0, MAX_CLIENTS * sizeof(struct pollfd));
    fds[0].fd = this->socket_fd;
    fds[0].events = POLLIN;
    fds_num = 1;

    while (1) {
        if (poll(fds, fds_num, -1) == -1) {
            std::cout << "Failed to poll" << std::endl;
            close(this->socket_fd);
            return;
        }

        for (int j = 0; j < fds_num; j++) 
        {
            if (fds[j].revents & POLLIN) 
            {
                if (fds[j].fd == this->socket_fd) 
                {
                    handleNewConnection(clientMap, fds, fds_num);
                } 
                else 
                {
                    handleClientMessage(clientMap, Channels, fds, fds_num, j);
                }
            }
        }
    }
}