/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 18:01:20 by mforest-          #+#    #+#             */
/*   Updated: 2025/12/17 18:01:20 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC
#define FT_IRC

#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <iostream>
#include <netdb.h>
#include <csignal>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "client.hpp"
#include "channel.hpp"
#include "command.hpp"
#include "server.hpp"

extern bool	g_stop_server;

void		setup_signals(void);

#endif
