/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_commands.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:00:58 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:00:58 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

void Server::handleNewConnection()
{
    struct sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);
    int new_fd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientSize);

    if (new_fd >= 0)
    {
        if (fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
        {
            perror("fcntl");
            close(new_fd);
            return;
        }

        struct pollfd p = {new_fd, POLLIN, 0};
        _fds.push_back(p);

        std::string ip = inet_ntoa(clientAddr.sin_addr);
        _clients.insert(std::make_pair(new_fd, Client(new_fd, ip)));

        std::cout << "[SERVER] New connection from " << ip << " on FD " << new_fd << std::endl;
    }
}

void Server::handleClientMessage(size_t i, char buf[512], int bytes)
{
    Client &client = _clients.at(_fds[i].fd);

    client.addBuffer(std::string(buf, bytes));

    std::string cmd_str;
    while (!(cmd_str = client.getNextCommand()).empty())
    {
        if (cmd_str.find("PONG") == std::string::npos)
             std::cout << "[DEBUG RECEIVE] FD " << client.getFd() << ": " << cmd_str << std::endl;

        Command parsedCmd = parse_cmd(cmd_str);
        dispatch(client, parsedCmd);
    }
}

void Server::removeClient(size_t i)
{
    int fd = _fds[i].fd;

    if (_clients.find(fd) != _clients.end())
    {
        Client &client = _clients.at(fd);
        std::string nickname = client.getNickname().empty() ? "unknown" : client.getNickname();

        if (client.isRegistered())
        {
            std::string msg = ":" + nickname + " QUIT :Connection closed\r\n";
            this->broadcast(msg, fd);
        }
        _clients.erase(fd);
    }

    std::cout << "[SERVER] Client disconnected (FD " << fd << ")" << std::endl;
    close(fd);
    _fds.erase(_fds.begin() + i);
}

void Server::dispatch(Client &client, const Command &cmd)
{
    if (client.isRegistered() == false)
    {
        if (cmd.name == "PASS" || cmd.name == "NICK" || cmd.name == "USER" || cmd.name == "CAP")
        {
            std::map<std::string, Handler>::iterator it = _dispatcher.find(cmd.name);
            if (it != _dispatcher.end())
                (this->*(it->second))(client, cmd);
            return;
        }

        sendError(client, 451, cmd.name);
        return;
    }

    std::map<std::string, Handler>::iterator it = _dispatcher.find(cmd.name);
    if (it != _dispatcher.end())
        (this->*(it->second))(client, cmd);
    else
        sendError(client, 421, cmd.name);
}
