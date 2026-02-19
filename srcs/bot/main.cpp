/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 02:29:25 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 02:29:25 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <host> <port> <password>" << std::endl;
        return (EXIT_FAILURE);
    }

    std::string host = argv[1];
    int port = std::atoi(argv[2]);
    if (port <= 0)
    {
        std::cerr << "Error: Invalid port" << std::endl;
        return (EXIT_FAILURE);
    }

	setup_signals();
    try
    {
        Bot bot("bot_IRC", "bot_IRC", argv[3]);
        bot.connectToServer(host, port);
        std::cout << "[Bot] Connected to " << host << ":" << port << std::endl;
        bot.run(); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return 0;
}

