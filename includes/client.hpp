/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 21:44:30 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/05 21:44:30 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
	public:
		Client();
		Client(int fd, std::string hostname);

		int	getFd() const;
		void getBuffer() const;
		std::string getHostname() const;
		std::string getUsername() const;
		std::string getRealname() const;
		std::string getNickname() const;
		bool isRegistered();
		bool isAuthentificated();

		void setNickname(const std::string nick);
		void setUsername(const std::string user);
		void setRealname(const std::string real);
		void setRegistered(bool state);
		void setAuthentificated(bool state);

		void	setPassed(bool current);
		void	registerUser(std::string username, std::string nickname, std::string realname);

		void	addBuffer(std::string input);
		std::string getNextCommand();

		~Client();
	private:
		int _fd;
	
		std::string _password;

		std::string _hostname;
		std::string _username;
		std::string _nickname;
		std::string _realname;
		std::string _buffer;

		bool _authentificated;
		bool _registered;
		bool _passed;
};

#endif
