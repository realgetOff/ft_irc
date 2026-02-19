/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 01:14:45 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 01:14:45 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "ft_irc.hpp"

struct Command
{
	std::string name;
	std::vector<std::string> params;
	std::string trailing;
};

#endif
