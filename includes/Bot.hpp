/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 02:37:26 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 02:37:26 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <signal.h>
#include <unistd.h>

struct Command
{
	std::string				prefix;
	std::string				name;
	std::vector<std::string>	params;
	std::string				trailing;
};

struct GameState
{
    std::vector<int> playerHand;
    std::vector<int> dealerHand;
    bool active;

    GameState();
    ~GameState();
};

class Bot
{
	private:
		int								_sock;
		std::string						_password;
		std::string						_nick;
		std::string						_user;
		std::string						_buffer;
		struct pollfd					_pfd;
		std::map<std::string, GameState>	_games;

		void		handleIncomingData();
		void		handleLine(const std::string& line);
		Command		parseCommand(const std::string& line);
		void		dispatch(const std::string& name, const Command& cmd);
		void		sendRaw(const std::string& msg);
		std::string	trim(const std::string& s);

		std::string	intToString(int n);
		int			calculateScore(const std::vector<int>& hand);
		std::string	handToString(const std::vector<int>& hand);

	public:
		Bot(const std::string& nick, const std::string& user, const std::string& password);
		~Bot();

		void	connectToServer(const std::string& host, int port);
		void	run();
};

void	setup_signals(void);

#endif
