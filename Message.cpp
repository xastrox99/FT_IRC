#include "Message.hpp"

Message::Message()
{
    // this->socket = 0;
    // this->params = std::vector<std::string>();
    // is_authenticated = false;
}

Message::Message(int socket)
{
    this->socket = socket;
    this->params = std::vector<std::string>();
    is_authenticated = false;
    this->client = Client(socket);
}

Message::~Message()
{
}

int Message::get_socket()
{
    return (this->socket);
}

std::string Message::get_prefix()
{
    return (this->prefix);
}

std::string Message::get_command()
{
    return (this->command);
}

std::vector<std::string> Message::get_params()
{
    return (this->params);
}

void Message::set_prefix(std::string prefix)
{
    this->prefix = prefix;
}

void Message::set_command(std::string command)
{
    this->command = command;
}

void Message::set_params(std::vector<std::string> params)
{
    this->params = params;
}
static std::vector<std::string> ft_split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

std::string Message::parss_password(std::string password, std::string buffer, std::vector<Client> &clients)
{
    message = buffer;
    if(!is_authenticated)
    {
        if(message.substr(0, 4) == "PASS")
        {
            if(client.get_pass())
                return (":localhost 462 USER :You may not reregister\r\n");
            else if(message.substr(5, password.length()) == password)
            {
                client.set_pass(true);
                return (":localhost 001 * :Welcome to the Internet Relay Network\r\n");
            }
            else if(message.empty())
            {
                return (":localhost 461 * PASS :Not enough parameters\r\n");
            }
            else
            {
                return (":localhost 464 * PASS :Password incorrect\r\n");
            }
        }
        else if (message.substr(0, 4) == "USER" && client.get_pass())
        {
            std::vector<std::string> split;
            split = ft_split(message, ' ');

            if (client.get_user())
                return (":localhost 462 USER :You may not reregister\r\n");
            else if(split.size() < 5)
                return (":localhost 461 * USER :Not enough parameters\r\n");
            else if ((split[2].size() == 1 && split[2][0] == '0') && (split[3].size() == 1 && split[3][0] == '*') && split[4][0] == ':')
            {
                // params.push_back(message.substr(0, pos));
                client.set_user(split[1], true);
                client.set_real_name(split[4].substr(1));
                std::cout<<client<<std::endl;
                clients.push_back(client);
                return ("");
            }
        }
        else if(message.substr(0, 4) == "NICK")
        {
            std::vector<std::string> split;
            split = ft_split(message, ' ');

        }
        else
        {
            return ("Not authenticated");
        }

    }
    
    return (":localhost 462 " /*+ client.get_nick()*/   " PASS :You may not reregister\r\n");

    // size_t pos = message.find(' ');
    // command = message.substr(0, pos);
    // message = message.substr(pos + 1);
    // std::cout << "command: " << command << std::endl;
    // std::cout << "message: " << message << std::endl;
    // if(pos == std::string::npos)
    // {
    //     if(command.find("PASS") != std::string::npos || command.find("USER") != std::string::npos)
    //     {
    //         std::cout << "Not enough arguments" << std::endl;
    //         return (-1);
    //     }
    //     else if(command.find("NICK") != std::string:: npos)
    //     {
    //         std::cout << "Error: No nickname given" << std::endl;
    //         return (-1);
    //     }
    //     else
    //     {
    //         std::cout << "Error: No command given" << std::endl;
    //         return (-1);
    //     }
    // }
    // // check_command(message, command, password, params.size());
}

// void Message::check_command(std::string message, std::string command, std::string password, int size)
// {
//     std::string tmp;
//     size_t newline = message.find('\n');
//     if(newline != std::string::npos)
//     {
//         tmp = message.substr(0, newline - 1);
//         if(tmp.empty())
//         {
//             std::cout << "Error : Empty message" << std::endl;
//             return;
//         }
//         else 
//             tmp = message;
//         if(size == 0)
//         {
//             if(command != "PASS")
//             {
//                 std::cout << "Error: Invalid format" << std::endl;
//                 return;
//             }
//             if(tmp != password)
//             {
//                 std::cout << "Error: Wrong password" << std::endl;
//                 return;
//             }
//             else
//             {
//                 std::cout << "Authenticated successfuly" << std::endl;
//                 password = tmp;
//             }
//         }
//         else 
//         {
//             if(command == "PASS" && tmp != password)
//             {
//                 std::cout << "Error: Wrong password" << std::endl;
//                 return;
//             }
//             else 
//             {
//                 std::cout << "Authenticated successfuly" << std::endl;
//                 password = tmp;
//             }
//         }
        
//     }
// }

void Message::handleError(int error)
{
    if(error == 462)
    {
        std::cout << "Error: Already registered" << std::endl;
        return;
    }
    else if(error == 464)
    {
        std::cout << "Error: Wrong password" << std::endl;
        close(socket);
        return;
    }
    else if(error == -1)
    {
        std::cout << "Error: Not authenticated" << std::endl;
        return;
    }
    else if (error == 461)
    {
        std::cout << "Error: Not enough arguments" << std::endl;
        return;
    }
    // else if (error == 0)
    // {
    //    // add client to list
    // }
}