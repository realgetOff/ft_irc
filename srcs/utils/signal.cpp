/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/05 02:17:22 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/05 02:17:22 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

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
