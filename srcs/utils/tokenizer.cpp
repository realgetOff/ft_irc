/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 17:53:03 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/28 17:53:03 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

Command Server::parse_cmd(std::string &cmd)
{
	struct Command cmds;
	std::istringstream iss(cmd);
	std::string token;

	if (!(iss >> cmds.name))
		return cmds;

	while (iss >> token)
	{
		if (token[0] == ':')
		{
			cmds.trailing = token.substr(1);

			std::string rest;
			std::getline(iss, rest);
			cmds.trailing += rest;
			break;
		}
		else
		{
			std::stringstream ss(token);
			std::string param;
			while (std::getline(ss, param, ','))
			{
				if (!param.empty())
					cmds.params.push_back(param);
			}

		}
	}
	return cmds;
}
