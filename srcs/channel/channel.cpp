/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:20:02 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:20:02 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "channel.hpp"
		
Channel::Channel(const std::string& name) :
	_name(name),
	_topic(""),
	_usercount(0),
	_isInviteOnly(false),
	_isTopicOnly(false),
	_isExternOnly(false),
	_isKeyOnly(false),
	_isLimitOnly(false),
	_key(""),
	_limit(0)
{};

void Channel::addUser(Client* client)
{
	Member m;
	m.client = client;
	m.isOp = _members.empty();
	_members[client->getFd()] = m;
	_usercount++;
}
