/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 21:42:43 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/05 21:42:43 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

Client::Client() :
	_fd(-1),
	_hostname(""),
	_authentificated(false),
	_registered(false)
{};

Client::Client(int fd, std::string hostname) : 
	_fd(fd),
	_hostname(hostname),
	_authentificated(false),
	_registered(false)
{};

int Client::getFd() const
{
	return (this->_fd);
}

std::string Client::getRealname() const
{
	return (this->_realname);
}

std::string Client::getHostname() const
{
	return (this->_hostname);
}

std::string Client::getNickname() const
{
	return (this->_nickname);
}

std::string Client::getUsername() const
{
	return (this->_username);
}

bool Client::isRegistered()
{
	if(this->_registered == false)
		return (false);
	else
		return (true);
}

bool Client::isAuthentificated()
{
	if(this->_authentificated == false)
		return(false);
	else
		return(true);
}

void Client::setRegistered(bool state)
{
	this->_registered = state;
}

void Client::setAuthentificated(bool state)
{
	this->_authentificated = state;
}

void Client::setNickname(const std::string nick)
{
	this->_nickname = nick;
}

void Client::setUsername(const std::string user)
{
	this->_username = user;
}

void Client::setRealname(const std::string real)
{
	this->_realname = real;
}

void Client::addBuffer(std::string data)
{
	this->_buffer += data;
}

std::string Client::getNextCommand(void)
{
	size_t pos = this->_buffer.find("\r\n");
	if(pos != std::string::npos)
	{
		std::string command = this->_buffer.substr(0, pos);
		this->_buffer.erase(0, pos + 2);
		return (command);
	}
	return("");
}

Client::~Client()
{};
