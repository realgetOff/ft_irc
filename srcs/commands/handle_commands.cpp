/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 00:54:51 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 00:54:51 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

void Server::handleCap(Client &client, const Command &cmd)
{
    if (!cmd.params.empty() && cmd.params[0] == "LS")
    {
        std::string reply = ":ircserv CAP * LS :\r\n";
        send(client.getFd(), reply.c_str(), reply.length(), 0);
        std::cout << "[DEBUG] CAP LS handled for FD " << client.getFd() << std::endl;
    }
}

void Server::handleKick(Client& client, const Command &cmd)
{
    if (cmd.params.size() < 2)
    {
        sendError(client, 461, cmd.name);
        return;
    }

    std::map<std::string, Channel*>::iterator it = _channels.find(cmd.params[0]);

    if (it == _channels.end())
    {
        sendError(client, 403, cmd.params[0]);
        return;
    }

    Channel* channel = it->second;

    if (!channel->isUser(client.getFd()))
    {
        sendError(client, 442, cmd.params[0]);
        return;
    }

    if (!channel->isOperator(client.getFd()))
    {
        sendError(client, 482, cmd.params[0]);
        return;
    }

    Client* target = channel->getClientByNick(cmd.params[1]);
    if (!target)
    {
        sendError(client, 441, cmd.name);
        return;
    }

    std::string reason = cmd.trailing.empty()
        ? client.getNickname()
        : cmd.trailing;

    std::string msg =
        ":" + client.getNickname() + "!" + client.getUsername() + "@localhost" + " KICK " +
        cmd.params[0] + " " + cmd.params[1] +
        " :" + reason + "\r\n";

    channel->broadcast(msg, -1);
    channel->removeUser(target->getFd());

}

void Server::handleMode(Client &client, const Command &cmd)
{
	if (cmd.params.size() < 1)
	{
        sendError(client, 461, cmd.name);
        return;
	}

	std::map<std::string, Channel*>::iterator it = _channels.find(cmd.params[0]);

    if (it == _channels.end())
    {
        sendError(client, 403, cmd.params[0]);
        return;
    }

    Channel* channel = it->second;

    if (!channel->isUser(client.getFd()))
    {
        sendError(client, 442, cmd.params[0]);
        return;
    }


	if (cmd.params.size() == 1)
	{
		std::string modes = channel->getModes();

		std::stringstream ss;

		ss << ":ircserv 324 " << (client.getNickname().empty() ?  "*" : client.getNickname())
			<< " " << channel->getName() << " +" << modes << "\r\n";
		std::string msg = ss.str();
		send(client.getFd(), msg.c_str(), msg.length(), 0);
		return ;
	}

	handleChannelModeChange(client, *channel, cmd);
}

void Server::handlePass(Client& client, const Command &cmd)
{
    if (client.isRegistered())
	{
        sendError(client, 462, cmd.name);
        return;
    }
    if (cmd.params.empty())
	{
        sendError(client, 461, cmd.name);
        return;
    }

    if (cmd.params[0] == this->_password)
    {
        client.setAuthentificated(true);
        std::cout << "[SERVER] Password OK for FD " << client.getFd() << std::endl;
    }
    else
    {
		sendError(client, 464, cmd.name);
		std::cout << "[SERVER] Bad password: got '" << cmd.params[0] << "' expected '" << _password << "'" << std::endl;

		close(client.getFd());
        return;
    }
}

void Server::handleNick(Client &client, const Command &cmd)
{
    if (cmd.params.empty())
	{
        sendError(client, 431, cmd.name);
        return;
    }

    std::string new_nick = cmd.params[0];
    std::string oldNick = client.getNickname();

    if (oldNick == new_nick)
		return;

    if (new_nick.size() < 3 || new_nick.size() >= 25)
	{
        std::string nickErr = ":ircserv NOTICE * :Nickname must be between 3 and 25 chars.\r\n";
        send(client.getFd(), nickErr.c_str(), nickErr.length(), 0);
        return;
    }

    if (isdigit(new_nick[0]) || new_nick[0] == '-')
	{
        sendError(client, 432, cmd.params[0]);
        return;
    }

    std::string allowedSpecial = "-_[]\\`^{|}";
    for (size_t i = 0; i < new_nick.size(); ++i)
	{
        if (!isalnum(new_nick[i]) && allowedSpecial.find(new_nick[i]) == std::string::npos)
		{
            sendError(client, 432, cmd.params[0]);
            return;
        }
    }

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
        if (strcasecmp(it->second.getNickname().c_str(), new_nick.c_str()) == 0)
		{
            if (it->first != client.getFd())
			{
                sendError(client, 433, cmd.params[0]);
                return;
            }
        }
    }

    client.setNickname(new_nick);
    if (!oldNick.empty())
        this->cleanupNickFromInvites(oldNick);

    if (client.isRegistered())
	{
        std::string msg = ":" + (oldNick.empty() ? "*" : oldNick) + "!" + client.getUsername() + "@localhost NICK :" + new_nick + "\r\n";

        send(client.getFd(), msg.c_str(), msg.length(), 0);

        std::map<std::string, Channel*>::iterator it = _channels.begin();
        for (; it != _channels.end(); ++it)
		{
            if (it->second->isUser(client.getFd()))
                it->second->broadcast(msg, client.getFd());
        }

        std::cout << "[INFO] " << oldNick << " is now known as " << new_nick << std::endl;
    }

    if (!client.isRegistered() && client.isAuthentificated() && !client.getUsername().empty())
        welcomeClient(client);
}


void Server::handleUser(Client &client, const Command &cmd)
{
	if (client.isRegistered())
	{
		sendError(client, 462, cmd.name);
		return;
	}
	if (cmd.params.size() < 3 || cmd.trailing.empty())
	{
		sendError(client, 461, cmd.name);
		return;
	}

	client.setUsername(cmd.params[0]);
	client.setRealname(cmd.trailing);

	if (!client.isRegistered() && client.isAuthentificated() && !client.getNickname().empty())
		welcomeClient(client);
}

void Server::handleQuit(Client &client, const Command &cmd)
{
    std::string reason = cmd.trailing.empty() ? "Client quit" : cmd.trailing;
    std::string quitMsg = ":" + client.getNickname() + "!" + client.getUsername()
                        + "@localhost QUIT :" + reason + "\r\n";

    std::map<std::string, Channel*>::iterator it = _channels.begin();
    while (it != _channels.end())
    {
        Channel *chan = it->second;

        if (chan->isUser(client.getFd()))
        {
            chan->broadcast(quitMsg, client.getFd());

            chan->removeUser(client.getFd());

            if (chan->isEmpty())
            {
                delete chan;
                _channels.erase(it++);
                continue;
            }
        }
        ++it;
    }
	this->cleanupNickFromInvites(client.getNickname());
	close(client.getFd());
}

void Server::handleJoinZero(Client &client, const Command &cmd)
{

		std::string identity = client.getNickname() + "!" + client.getUsername() + "@localhost";
		std::string reason = cmd.trailing.empty() ? "Leaving" : cmd.trailing;

		std::map<std::string, Channel*>::iterator it = _channels.begin();
		while (it != _channels.end())
		{
			Channel *chan = it->second;

			if (chan->isUser(client.getFd()))
			{
				std::string partMsg = ":" + identity + " PART " + it->second->getName() + " :" + reason + "\r\n";

				chan->broadcast(partMsg, -1);
				chan->removeUser(client.getFd());

				if (chan->isEmpty())
				{
					delete chan;
					_channels.erase(it++);
					continue;
				}
			}
			++it;
		}
		return ;

}

void	Server::handleJoin(Client &client, const Command &cmd)
{
	std::vector<std::string>	chans;
	std::vector<std::string>	keys;
	std::string					prefix;
	std::string					target;
	std::string					key;
	Channel						*chan;

	if (!client.isRegistered())
		return ;
	if (cmd.params.empty())
	{
		sendError(client, 461, cmd.name);
		return ;
	}
	if (cmd.params[0] == "0")
	{
		handleJoinZero(client, cmd);
		return ;
	}
	for (size_t i = 0; i < cmd.params.size(); ++i)
	{
		if (cmd.params[i][0] == '#')
			chans.push_back(cmd.params[i]);
		else
			keys.push_back(cmd.params[i]);
	}
	for (size_t i = 0; i < chans.size(); ++i)
	{
		target = chans[i];
		key = (i < keys.size()) ? keys[i] : "";
		if (target.size() < 2 || target.size() >= 25)
			continue ;
		std::map<std::string, Channel*>::iterator it = _channels.find(target);
		if (it == _channels.end())
		{
			chan = new Channel(target);
			_channels[target] = chan;
			chan->addUser(&client);
		}
		else
		{
			chan = it->second;
			if (chan->isUser(client.getFd()))
				continue ;
			if (chan->getIsInviteOnly() && !chan->isInvited(client.getNickname()))
			{
				sendError(client, 473, target);
				continue ;
			}
			if (chan->getModes().find('k') != std::string::npos && chan->getKey() != key)
			{
				sendError(client, 475, target);
				continue ;
			}
			if (chan->getModes().find('l') != std::string::npos && chan->getUserCount() >= chan->getLimit())
			{
				sendError(client, 471, target);
				continue ;
			}
			chan->addUser(&client);
		}
		prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";
		chan->broadcast(prefix + " JOIN " + target + "\r\n", 0);

		std::string nick = client.getNickname();
		if (!chan->getTopic().empty())
		{
			std::string msg = ":ircserv 332 " + nick + " " + target + " :" + chan->getTopic() + "\r\n";
			send(client.getFd(), msg.c_str(), msg.length(), 0);
		}
		std::string nmsg = ":ircserv 353 " + nick + " = " + target + " :" + chan->getNamesList() + "\r\n";
		send(client.getFd(), nmsg.c_str(), nmsg.length(), 0);
		std::string emsg = ":ircserv 366 " + nick + " " + target + " :End of /NAMES list\r\n";
		send(client.getFd(), emsg.c_str(), emsg.length(), 0);

		chan->removeInvite(client.getNickname());
	}
}

void Server::handleWho(Client &client, const Command &cmd)
{
    if (cmd.params.empty())
	{
		sendError(client, 461, cmd.name);
		return;
	}

    std::string target = cmd.params[0];
    std::string nick = client.getNickname();
    if (target[0] == '#')
    {
        std::map<std::string, Channel*>::iterator it = _channels.find(target);
        if (it != _channels.end())
        {
            std::map<int, Client>::iterator cit = _clients.begin();
            for (; cit != _clients.end(); ++cit)
            {
                if (it->second->isUser(cit->first))
                {
                    Client &c = cit->second;
                    std::string status = it->second->isOperator(c.getFd()) ? "H@" : "H";

                    std::string reply = ":ircserv 352 " + nick + " " + target + " " + c.getUsername()
                                      + " localhost ircserv " + c.getNickname() + " " + status
                                      + " :0 " + c.getRealname() + "\r\n";
                    send(client.getFd(), reply.c_str(), reply.length(), 0);
                }
            }
        }
    }
	else
	{
		Client *targetClient = findClientByNick(target);
        if (targetClient)
        {
            std::string status = "H";
            std::string reply = ":ircserv 352 " + nick + " * " + targetClient->getUsername()
                              + " localhost ircserv " + targetClient->getNickname() + " " + status
                              + " :0 " + targetClient->getRealname() + "\r\n";
            send(client.getFd(), reply.c_str(), reply.length(), 0);
        }
	}

    std::string endReply = ":ircserv 315 " + nick + " " + target + " :End of /WHO list\r\n";
    send(client.getFd(), endReply.c_str(), endReply.length(), 0);
}


void Server::handleWhois(Client &client, const Command &cmd)
{
    if (cmd.params.empty())
	{
        sendError(client, 431, cmd.name);
        return;
    }

    std::string targetNick = cmd.params[0];
    Client *target = findClientByNick(targetNick);

    if (!target)
	{
        sendError(client, 401, cmd.params[0]);
        return;
    }

    std::string nick = client.getNickname();

    std::string rpl311 = ":ircserv 311 " + nick + " " + target->getNickname() + " " +
                         target->getUsername() + " localhost * :" + target->getRealname() + "\r\n";
    send(client.getFd(), rpl311.c_str(), rpl311.length(), 0);

    std::string rpl318 = ":ircserv 318 " + nick + " " + targetNick + " :End of /WHOIS list\r\n";
    send(client.getFd(), rpl318.c_str(), rpl318.length(), 0);
}

void Server::handleInvite(Client &client, const Command &cmd)
{
    if (cmd.params.size() < 2)
	{
        sendError(client, 461, cmd.name);
        return;
    }

    std::string targetNick = cmd.params[0];
    std::string chanName = cmd.params[1];

    Client *target = findClientByNick(targetNick);
    if (!target)
	{
        sendError(client, 401, cmd.params[0]);
        return;
    }

    std::map<std::string, Channel*>::iterator it = _channels.find(chanName);
    if (it == _channels.end())
		return;

    Channel *chan = it->second;

    if (!chan->isUser(client.getFd()))
	{
        sendError(client, 442, cmd.params[0]);
        return;
    }

    if (chan->isUser(target->getFd()))
	{
        sendError(client, 443, cmd.params[0]);
        return;
    }

    if (chan->getIsInviteOnly() && !chan->isOperator(client.getFd()))
	{
        sendError(client, 482, cmd.params[0]);
        return;
    }

    chan->addInvite(targetNick);
    std::string rpl341 = ":ircserv 341 " + client.getNickname() + " " + targetNick + " " + chanName + "\r\n";
    send(client.getFd(), rpl341.c_str(), rpl341.length(), 0);

    std::string inviteMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + targetNick + " :" + chanName + "\r\n";
    send(target->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

void Server::handleTopic(Client &client, const Command &cmd)
{
    if (cmd.params.empty())
    {
        sendError(client, 461, cmd.name);
        return;
    }

    std::string chanName = cmd.params[0];
    std::map<std::string, Channel*>::iterator it = _channels.find(chanName);

    if (it == _channels.end())
    {
        sendError(client, 403, cmd.params[0]);
        return;
    }

    Channel *chan = it->second;

    if (!chan->isUser(client.getFd()))
    {
        sendError(client, 442, cmd.params[0]);
        return;
    }

    if (cmd.params.size() == 1 && cmd.trailing.empty())
    {
        if (chan->getTopic().empty())
        {
            std::string rpl = ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n";
            send(client.getFd(), rpl.c_str(), rpl.length(), 0);
        }
        else
        {
            std::string rpl = ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n";
            send(client.getFd(), rpl.c_str(), rpl.length(), 0);
        }
        return;
    }

	if (chan->getIsTopicOnly() && !chan->isOperator(client.getFd()))
    {
        sendError(client, 482, cmd.params[0]);
        return;
    }

    std::string newTopic;
    if (cmd.params.size() == 1 && !cmd.trailing.empty())
		newTopic = cmd.trailing;


    chan->setTopic(newTopic);

    std::string notify = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + chanName + " :" + newTopic + "\r\n";
    chan->broadcast(notify, -1);
}

void Server::handlePart(Client &client, const Command &cmd)
{
    if (!client.isRegistered())
        return;

    if (cmd.params.empty())
    {
        sendError(client, 461, cmd.name);
        return;
    }

    std::string chanName = cmd.params[0];
    std::map<std::string, Channel*>::iterator it = _channels.find(chanName);

    if (it == _channels.end())
    {
        sendError(client, 403, cmd.params[0]);
        return;
    }

    Channel *chan = it->second;
    if (!chan->isUser(client.getFd()))
    {
        sendError(client, 442, cmd.params[0]);
        return;
    }

    std::string identity = client.getNickname() + "!" + client.getUsername() + "@localhost";
    std::string reason = cmd.trailing.empty() ? "Leaving" : cmd.trailing;
    std::string partMsg = ":" + identity + " PART " + chanName + " :" + reason + "\r\n";

    it->second->broadcast(partMsg, -1);
    it->second->removeUser(client.getFd());

    if (it->second->isEmpty())
	{
        delete it->second;
        _channels.erase(it);
    }
}

void Server::handleNotice(Client &client, const Command &cmd)
{
	if (!client.isRegistered())
        return;

    if (cmd.params.empty())
        return;

    if (cmd.trailing.empty())
		return;

	std::string target = cmd.params[0];
    std::string prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";
    std::string noticeMsg = prefix + " NOTICE " + target + " :" + cmd.trailing + "\r\n";

	if (target[0] == '#')
	{
		std::map<std::string, Channel*>::iterator it = _channels.find(target);

		if (it != _channels.end())
		{
			Channel *chan = it->second;
			if (chan->isUser(client.getFd()))
				chan->broadcast(noticeMsg, client.getFd());
		}
	}
	else
	{
		Client *targetClient = findClientByNick(target);
		if (targetClient)
			send(targetClient->getFd(), noticeMsg.c_str(), noticeMsg.length(), 0);
	}
}

void Server::handlePrivmsg(Client &client, const Command &cmd)
{
    if (!client.isRegistered())
        return;

    if (cmd.params.empty())
    {
        sendError(client, 411, cmd.name);
        return;
    }

    if (cmd.trailing.empty())
    {
        sendError(client, 412, cmd.name);
        return;
    }
	std::string prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";

	for(size_t i = 0; i < cmd.params.size(); i++)
	{
    std::string target = cmd.params[i];
    std::string fullMsg = prefix + " PRIVMSG " + target + " :" + cmd.trailing + "\r\n";

    if (target[0] == '#')
    {
        std::map<std::string, Channel*>::iterator it = _channels.find(target);

        if (it == _channels.end())
        {
            sendError(client, 403, cmd.params[0]);
            return;
        }

        if (!it->second->isUser(client.getFd()))
        {
            sendError(client, 404, cmd.params[0]);
            return;
        }

        it->second->broadcast(fullMsg, client.getFd());
    }
    else
    {
        bool found = false;
        std::map<int, Client>::iterator it = _clients.begin();

        for (; it != _clients.end(); ++it)
        {
            if (it->second.getNickname() == target)
            {
                send(it->first, fullMsg.c_str(), fullMsg.length(), 0);
                found = true;
                break;
            }
        }
        if (!found)
            sendError(client, 401, cmd.params[0]);
    }
	}
}
