#include <SFML\Network.hpp>
#include <iostream>

#define BUFF_SIZE 128

#include "AuthKit.hpp"
#include "Config.hpp"
#include "Endpoints.hpp"

// globals
bool running = true;

sf::UdpSocket socket;

config* configuration;

/**
 * take request and route to correct endpoint handle
*/
void route(char* data, sf::IpAddress& sender, unsigned short& port)
{ 
    // pinged
    if (data[0] == '!') endpoint::ping(sender, port, socket);

    // check if packet is a request
    else if (data[0] == 'r')
    {
        // request lobby count
        if (data[1] == 'l' && data[2] == 'c') endpoint::lobby_count(sender, port, configuration);

        // request lobby details
        if (data[1] == 'l' && data[2] == 'd') endpoint::lobby_details(data, sender, port, configuration);
    }
    // client is requesting response stack
    else endpoint::get_stack(data, sender, port, socket);
}

int main(int argc, char **argv)
{
    // variables needed
    char data[BUFF_SIZE];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    // load config
    configuration = new config("./config.json");

    // bind to port. exit if failed.
    if (socket.bind(configuration->port) != sf::Socket::Done) return 1;

    // loop recieving packets
    while (running)
    {
        // recieve message and die if an error occurs
        if (socket.receive(data, BUFF_SIZE, received, sender, port) != sf::Socket::Done) return 1;

        // if no prefix, check auth and make connection
        if (data[0] == '-') authkit::check(data, sender, port);

        // else handle request
        else route(data, sender, port);
    }

    return 0;
}