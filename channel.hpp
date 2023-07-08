#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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

class channel
{
private:
    std::map<std::string ,  >
public:
    channel();
    ~channel();
};

channel::channel()
{
}

channel::~channel()
{
}


#endif