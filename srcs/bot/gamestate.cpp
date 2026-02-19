/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gamestate.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 03:26:55 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 03:26:55 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

GameState::GameState() :
	active(false)
{
	playerHand.clear();
	dealerHand.clear();
}

GameState::~GameState()
{};
