/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_bot.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 03:21:19 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 03:21:19 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

bool	g_stop_server = false;

void	handle_sig(int sig)
{
	(void)sig;
	g_stop_server = true;
}

void	setup_signals(void)
{
	signal(SIGINT, handle_sig);
	signal(SIGPIPE, SIG_IGN);
}
