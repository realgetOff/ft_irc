/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_ops.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 00:53:41 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 00:53:41 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

void Server::run()
{
    while (g_stop_server == false)
    {
        if (poll(_fds.data(), _fds.size(), -1) < 0)
        {
            if (g_stop_server)
				break;
			if (_serverSocket != -1)
				close(_serverSocket);
            throw std::runtime_error("Error: poll failed");
        }

        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                this->removeClient(i);
                i--;
                continue;
            }

            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _serverSocket)
                    this->handleNewConnection();
                else
                {
                    char buf[512];
                    int bytes = recv(_fds[i].fd, buf, sizeof(buf) - 1, 0);

                    if (bytes <= 0)
                    {
                        this->removeClient(i);
                        i--;
                    }
                    else
                        this->handleClientMessage(i, buf, bytes);
                }
            }
        }
    }
}
