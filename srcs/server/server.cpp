/* ************************************************************************** */
/* */
/* server.cpp                                                              */
/* */
/* ************************************************************************** */

#include "ft_irc.hpp"

Server::Server(int port, std::string pass) :
	_password(pass),
	_port(port),
	_serverSocket(-1)
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("Error: socket creation failed");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		if (_serverSocket != -1)
			close(_serverSocket);
		throw std::runtime_error("Error: setsockopt failed");
	}

    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
	{
    	if (_serverSocket != -1)
			close(_serverSocket);
		throw std::runtime_error("Error: fcntl failed");
	}

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
	    if (_serverSocket != -1)
			close(_serverSocket);
		throw std::runtime_error("Error: bind failed");
	}

    if (listen(_serverSocket, SOMAXCONN) < 0)
	{
    	if (_serverSocket != -1)
			close(_serverSocket);
		throw std::runtime_error("Error: listen failed");
	}

    struct pollfd p;
    p.fd = _serverSocket;
    p.events = POLLIN;
    p.revents = 0;
    _fds.push_back(p);

    _dispatcher["PASS"] = &Server::handlePass;
    _dispatcher["NICK"] = &Server::handleNick;
    _dispatcher["USER"] = &Server::handleUser;
    _dispatcher["QUIT"] = &Server::handleQuit;
    _dispatcher["JOIN"] = &Server::handleJoin;
    _dispatcher["PRIVMSG"] = &Server::handlePrivmsg;
	_dispatcher["CAP"] = &Server::handleCap;
	_dispatcher["PART"] = &Server::handlePart;
	_dispatcher["KICK"] = &Server::handleKick;
	_dispatcher["WHO"] = &Server::handleWho;
	_dispatcher["whois"] = &Server::handleWhois;
	_dispatcher["INVITE"] = &Server::handleInvite;
	_dispatcher["TOPIC"] = &Server::handleTopic;
	_dispatcher["NOTICE"] = &Server::handleNotice;
	_dispatcher["MODE"] = &Server::handleMode;

    std::cout << "[SERVER] Listening on port " << _port << "..." << std::endl;
}

Server::~Server()
{
    for (size_t i = 0; i < _fds.size(); ++i)
        close(_fds[i].fd);
    _clients.clear();
    _fds.clear();

	std::map<std::string, Channel*>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();

    if (_serverSocket != -1)
        close(_serverSocket);
        
    std::cout << "[SERVER] Shutdown successfully." << std::endl;
}
