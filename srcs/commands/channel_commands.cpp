/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel_commands.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:17:32 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:17:32 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

std::string Channel::getName() const
{
	return (this->_name);
}

std::string Channel::getNamesList() const
{
    std::string list;
    std::map<int, Member>::const_iterator it;
    for (it = _members.begin();  it != _members.end(); ++it)
    {
        if(it->second.isOp)
            list += "@";
        list += it->second.client->getNickname() + " ";
    }
    return list;
}

void Channel::removeUser(int fd)
{
	bool wasOp = true;

	std::map<int, Member>::iterator it = _members.find(fd);
	if (it == _members.end())
		return ;

	wasOp = it->second.isOp;
	_members.erase(it);

	if (wasOp && !_members.empty())
	{
		std::string channelName = this->getName();
		std::string targetNick = _members.begin()->second.client->getNickname();

		std::string opMsg = ":ircserv MODE " + channelName + " +o " + targetNick + "\r\n";
		broadcast(opMsg, -1);
		_members.begin()->second.isOp = true;
	}
	_usercount--;
}

bool Channel::isEmpty() const
{
	return _members.empty();
}

void Channel::broadcast(const std::string& message, int excludeFd)
{
    std::map<int, Member>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it)
	{
        if (it->first != excludeFd)
		{
			send(it->first, message.c_str(), message.length(), 0);
		}
    }
}

bool Channel::isUser(int fd) const
{
	return _members.find(fd) != _members.end();
}

bool Channel::isOperator(int fd) const
{
	std::map<int, Member>::const_iterator it = _members.find(fd);

	if (it == _members.end())
		return false;

	return it->second.isOp;
}

void Channel::setOperator(int fd, bool value)
{
	_members.find(fd)->second.isOp = value;
}

Client* Channel::getClientByNick(const std::string& nick) const
{
    for (std::map<int, Member>::const_iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (it->second.client->getNickname() == nick)
            return it->second.client;
    }
    return NULL;
}

void Channel::clearInvitationsForNick(const std::string &nick)
{
    for (std::list<std::string>::iterator it = _inviteList.begin(); it != _inviteList.end(); )
	{
        if (*it == nick)
            it = _inviteList.erase(it);
		else
            ++it;
    }
}

void Channel::addInvite(const std::string &nick)
{
    for (std::list<std::string>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it)
	{
        if (*it == nick)
			return;
    }
	_inviteList.push_back(nick);
}

bool Channel::isInvited(const std::string &nick)
{
    for (std::list<std::string>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it)
	{
        if (*it == nick)
            return true;
    }
    return false;
}

void Channel::removeInvite(const std::string &nick)
{
    for (std::list<std::string>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it)
	{
        if (*it == nick)
		{
            _inviteList.erase(it);
            return;
        }
    }
}

bool Channel::getIsInviteOnly(void) const
{
	return (this->_isInviteOnly);
}

void Channel::setIsInviteOnly(bool state)
{
	this->_isInviteOnly = state;
}

bool Channel::getIsTopicOnly(void) const
{
	return (this->_isTopicOnly);
}

void Channel::setIsTopicOnly(bool state)
{
	this->_isTopicOnly = state;
}

std::string Channel::getTopic(void) const
{
	return (this->_topic);
}

void Channel::setTopic(std::string newTopic)
{
	this->_topic = newTopic;
}

bool Channel::getIsExternOnly(void) const
{
	return (this->_isExternOnly);
}

void Channel::setIsExternOnly(bool state)
{
	this->_isExternOnly = state;
}

void Channel::setKey(const std::string& key)
{
	this->_isKeyOnly = true;
	this->_key = key;
}

const std::string Channel::getKey() const
{
	return this->_key;
}

void Channel::clearKey()
{
	this->_isKeyOnly = false;
	this->_key = "";
}

void Channel::setLimit(const int limit)
{
	this->_isLimitOnly = true;
	this->_limit = limit;
}

int Channel::getLimit() const
{
	return this->_limit;
}

void Channel::clearLimit()
{
	this->_isLimitOnly = false;
	this->_limit = 0;
}

int Channel::getUserCount() const
{
	return this->_usercount;
}

std::string Channel::getModes() const
{
	std::string modes = "";
	if (this->_isInviteOnly)
		modes += "i";
	if (this->_isTopicOnly)
		modes += "t";
	if (this->_isExternOnly)
		modes += "n";
	if (this->_isKeyOnly)
		modes += "k";
	if (this->_isLimitOnly)
		modes += "l";
	return modes;
}
