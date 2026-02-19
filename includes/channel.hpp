/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 01:15:35 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/15 01:15:35 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
	private:
		struct Member
		{
			Client* client;
			bool    isOp;
		};
		std::string _name;
		std::string _topic;
		int	_usercount;

		std::list<std::string> _inviteList;
		std::map<int, Member> _members;
		bool _isInviteOnly;
		bool _isTopicOnly;
		bool _isExternOnly;
		bool _isKeyOnly;
		bool _isLimitOnly;
		std::string _key;
		int	_limit;

	public:
		std::string getNamesList() const;

		Client* getClientByNick(const std::string& nick) const;

		Channel(const std::string& name);
		void broadcast(const std::string& message, int excludeFd);

		std::string getName() const;
		void clearInvitationsForNick(const std::string &nick);
		void addInvite(const std::string &nick);
		bool isInvited(const std::string &nick);
		void removeInvite(const std::string &nick);
		void addUser(Client* client);
		void removeUser(int fd);
		bool isUser(int fd) const;
		bool isOperator(int fd) const;
		bool isEmpty() const;

		bool getIsInviteOnly(void) const;
		void setIsInviteOnly(bool state);
		bool getIsTopicOnly(void) const;
		void setIsTopicOnly(bool state);
		bool getIsExternOnly(void) const;
		void setIsExternOnly(bool state);
		const std::string getKey(void) const;
		void clearKey();
		void setKey(const std::string& key);
		int getLimit(void) const;
		void clearLimit();
		void setLimit(const int limit);
		int	getUserCount() const;
		std::string getModes() const;

		std::string getTopic(void) const;
		void setTopic(std::string newTopic);

		void setOperator(int fd, bool value);
};

#endif
