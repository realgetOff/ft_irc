/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 17:58:25 by mforest-          #+#    #+#             */
/*   Updated: 2025/12/17 17:58:25 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return (EXIT_FAILURE);
    }
    int port = std::atoi(av[1]);
	if(port < 1 || port > 65535)
	{
		std::cerr << "Error: Port must be on valid TCP range (1-65535)" << std::endl; 
		return (EXIT_FAILURE);
	}
    std::string pass = av[2];

	setup_signals();

    try
    {
        Server myServer(port, pass);
        myServer.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (0);
}
