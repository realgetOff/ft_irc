/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:13:57 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:13:57 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class Channel;

class Server
{
	public:
	Server(int port, std::string password);

	void dispatch(Client &client, const Command &cmd);

	void welcomeClient(Client &client);
	void handleNick(Client &client, const Command &cmd);
	void handleJoin(Client &client, const Command &cmd);
	void handlePrivmsg(Client &client, const Command &cmd);
	void handleUser(Client &client, const Command &cmd);
	void handleQuit(Client &client, const Command &cmd);
	void handlePass(Client &client, const Command &cmd);
	void handleCap(Client &client, const Command &cmd);
	void handlePart(Client &client, const Command &cmd);
	void handleWho(Client &client, const Command &cmd);
	void handleWhois(Client &client, const Command &cmd);
	void handleKick(Client &client, const Command &cmd);
	void handleInvite(Client &client, const Command &cmd);
	void handleTopic(Client &client, const Command &cmd);
	void handleNotice(Client &client, const Command &cmd);
	void handleMode(Client &client, const Command &cmd);
	void processJoin(Client &client, const Command &cmd, std::string key);
	void handleChannelModeChange(Client &client, Channel& channel, const Command &cmd);
	void handleJoinZero(Client &client, const Command &cmd);
	void cleanupNickFromInvites(const std::string &oldNick);

	void sendNames(Client &client, Channel *chan);

	void broadcast(const std::string &message, int excludeFd);

	void parseAndApplyModes(Client& client, Channel& channel, const Command &cmd);
	bool applyChannelModes(Client& client, Channel& channel,
			char mode, bool add, const Command& cmd, size_t& paramIndex);

	Command parse_cmd(std::string &cmd);

	Client *findClientByNick(const std::string &nick);
	void sendError(Client &client, int err_code, std::string target_param);

	int initServer();
	void run();

	void handleNewConnection();
	void handleClientMessage(size_t index, char buf[512], int bytes);
	void removeClient(size_t index);

	~Server();
  private:
	const std::string _password;
	int _port;
	int _serverSocket;
	std::vector<struct pollfd> _fds;
	std::map<int, Client> _clients;

	typedef void (Server::*Handler)(Client&, const Command&);
	std::map<std::string, Channel*> _channels;
	std::map<std::string, Handler> _dispatcher;
};

#endif
