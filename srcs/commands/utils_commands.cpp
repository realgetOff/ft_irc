/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_commands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:06:10 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:06:10 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

void Server::sendError(Client &client, int err_code, std::string target_param)
{
    std::stringstream ss;
    std::string target = client.getNickname().empty() ? "*" : client.getNickname();

    ss << ":ircserv " << err_code << " " << target;
    switch (err_code)
    {
        case 401: ss << " " << target_param << " :No such nick/channel";
				  break;
		case 403: ss << " :No such channel";
				  break;
		case 404: ss << " :Cannot send to channel";
				  break;
		case 411: ss << " :No recipient given";
				  break;
		case 412: ss << " :No text to send";
				  break;
        case 421: ss << " " << target_param << " :Unknown command";
				  break;
		case 431: ss << " :No nickname given";
				  break;
        case 433: ss << " " << target_param << " :Nickname is already in use";
				  break;
		case 441: ss << " " << target_param << " :They aren't on that channel";
				  break;
		case 442: ss << target_param << " :Your not in that channel";
				  break;
		case 443: ss << target_param <<" :Is already on channel";
				  break;
        case 451: ss << " :You have not registered";
				  break;
        case 461: ss << " " << target_param << " :Not enough parameters";
				  break;
        case 462: ss << " :Unauthorized command (already registered)";
				  break;
        case 464: ss << " :Password incorrect";
				  break;
		case 471: ss << " " << target_param << " :Channel is full";
				  break;
		case 472: ss << " " << target_param << " :is unknown mode char to me";
                  break;
		case 473: ss << " " << target_param << " :Cannot join channel (+i)";
				  break;
        case 475: ss << " " << target_param << " :Cannot join channel (+k)";
				  break;
		case 482: ss << target_param << " :You're not channel operator";
				  break;
        default:  ss << " :Unknown error";
				  break;
    }
    ss << "\r\n";
    std::string msg = ss.str();
    send(client.getFd(), msg.c_str(), msg.length(), 0);
}

void Server::broadcast(const std::string &message, int excludeFd)
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->first != excludeFd)
			send(it->first, message.c_str(), message.length(), 0);
    }
}

Client *Server::findClientByNick(const std::string &nick)
{
    std::map<int, Client>::iterator it = _clients.begin();

    for (; it != _clients.end(); ++it)
    {
        if (it->second.getNickname() == nick)
            return &(it->second);
    }
    return NULL;
}

void Server::sendNames(Client &client, Channel *chan)
{
	std::string nick = client.getNickname();
	std::string chanName = chan->getName();

	std::string reply = ":ircserv 353 " + nick + " = " + chanName + " :" + chan->getNamesList() + "\r\n";
	send(client.getFd(), reply.c_str(), reply.length(), 0);

	std::string end = ":ircserv 366 " + nick + " " + chanName + " :End of /NAMES list\r\n";
	send(client.getFd(), end.c_str(), end.length(), 0);
}

void Server::cleanupNickFromInvites(const std::string &oldNick)
{
    std::map<std::string, Channel*>::iterator it = _channels.begin();

    for (; it != _channels.end(); ++it)
        it->second->clearInvitationsForNick(oldNick);
}
