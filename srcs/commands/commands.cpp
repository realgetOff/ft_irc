/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 00:54:10 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 00:54:10 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

void Server::welcomeClient(Client &client)
{
	if (client.isRegistered() == true || client.isAuthentificated() == false)
		return;

	client.setRegistered(true);

    std::string nick = client.getNickname();
    std::string reply = ":ircserv 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + client.getUsername() + "@localhost\r\n";

    send(client.getFd(), reply.c_str(), reply.length(), 0);

	std::cout << "[SERVER] Client " << nick << " successfully registered!" << std::endl;
}

void Server::handleChannelModeChange(Client &client, Channel& channel, const Command &cmd)
{
	if (!channel.isOperator(client.getFd()))
	{
		sendError(client, 482, cmd.params[0]);
		return;
	}
	parseAndApplyModes(client, channel, cmd);
}

void Server::parseAndApplyModes(Client& client, Channel& channel, const Command &cmd)
{
	bool adding = true;
	size_t paramIndex = 2;

	const std::string& modes = cmd.params[1];
	for (size_t i = 0; i < modes.size(); i++)
	{
		char c = modes[i];

		if (c == '+')
		{
			adding = true;
			continue;
		}
		if (c == '-')
		{
			adding = false;
			continue;
		}

		if (!applyChannelModes(client, channel, c, adding, cmd, paramIndex))
			return ;
	}
	std::stringstream ss;

	ss << ":" << "ircserv" << " MODE " << channel.getName() << " " << cmd.params[1];
	for (size_t i = 2; i < paramIndex; i++)
		ss << " " << cmd.params[i];
	channel.broadcast(ss.str() + "\r\n", -1);
}

bool Server::applyChannelModes(Client& client, Channel& channel,
			char mode, bool add, const Command& cmd, size_t& paramIndex)
{
	switch (mode)
	{
		case 'i': channel.setIsInviteOnly(add);
				  break;
		case 't': channel.setIsTopicOnly(add);
				  break;
		case 'n': channel.setIsExternOnly(add);
				  break;
		case 'o': 
		  {
				if (paramIndex >= cmd.params.size())
				{
					sendError(client, 461, cmd.name);
					return false;
				}
				const std::string& nick = cmd.params[paramIndex++];
				Client* target = channel.getClientByNick(nick);
				if (!target || !channel.isUser(target->getFd()))
				{
					sendError(client, 441, cmd.name);
					return false;
				}
				channel.setOperator(target->getFd(), add);
				break ;
			}
		case 'k' :
			{
				if (add)
					{
						if (paramIndex >= cmd.params.size())
						{
							sendError(client, 461, cmd.name);
							return false;
						}
						channel.setKey(cmd.params[paramIndex++]);
					}
				else
					channel.clearKey();
				break ;
			}
		case 'l' :
			{
				if (add)
				{
					if (paramIndex >= cmd.params.size())
					{
						sendError(client, 461, cmd.name);
						return false;
					}
					int limit = std::atoi(cmd.params[paramIndex++].c_str());
					if (limit <= 0)
					{
						sendError(client, 461, cmd.name);
						return false;
					}
					channel.setLimit(limit);
				}
				else
					channel.clearLimit();
				break ;
			}
		default:
			{
				sendError(client, 472, cmd.params[0]);
				return false;
			}
	}
	return true;
}
