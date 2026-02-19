/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mforest- <marvin@d42.fr>                   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 02:26:26 by mforest-          #+#    #+#             */
/*   Updated: 2026/01/19 02:26:26 by mforest-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Bot.hpp"

Bot::Bot(const std::string& nick, const std::string& user, const std::string& password) :
	_sock(-1),
	_password(password),
	_nick(nick),
	_user(user)
{};

Bot::~Bot()
{
    if (_sock != -1)
        close(_sock);
}

void Bot::connectToServer(const std::string& host, int port)
{
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0)
		throw std::runtime_error("socket failed");

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(_sock, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		if (_sock != -1)
        	close(_sock);
		throw std::runtime_error("connect failed");
	}

    _pfd.fd = _sock;
    _pfd.events = POLLIN;

	if (!_password.empty())
		sendRaw("PASS " + _password);
    sendRaw("NICK " + _nick);
    sendRaw("USER " + _user + " 0 * :" + _user);
}

void Bot::run()
{
    while (true)
    {
        int ret = poll(&_pfd, 1, -1);
        if (ret < 0)
            throw std::runtime_error("poll failed");

        if (_pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
            throw std::runtime_error("server disconnected");

        if (_pfd.revents & POLLIN)
        {
            char buf[512];
            int bytes = recv(_sock, buf, sizeof(buf), 0);
            if (bytes <= 0)
                throw std::runtime_error("connection closed");

            _buffer.append(buf, bytes);
            handleIncomingData();
        }
    }
}

void Bot::handleIncomingData()
{
    size_t pos;
    while ((pos = _buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 2);
        handleLine(line);
    }
}

void Bot::handleLine(const std::string& line)
{
    Command cmd = parseCommand(line);

    if (cmd.name == "PING")
    {
        sendRaw("PONG :" + cmd.trailing);
        return;
    }

    if (cmd.name == "001")
    {
        sendRaw("JOIN #bot");
        return;
    }

    dispatch(cmd.name, cmd);
}

Command Bot::parseCommand(const std::string& line)
{
    Command cmd;
    std::string input = trim(line);
    size_t pos = 0;

    if (!input.empty() && input[0] == ':')
    {
        pos = input.find(' ');
        if (pos == std::string::npos)
            return cmd;
        pos++;
    }

    size_t end = input.find(' ', pos);
    if (end == std::string::npos)
    {
        cmd.name = input.substr(pos);
        return cmd;
    }

    cmd.name = input.substr(pos, end - pos);
    pos = end + 1;

    while (pos < input.size())
    {
        if (input[pos] == ':')
        {
            cmd.trailing = input.substr(pos + 1);
            break;
        }

        end = input.find(' ', pos);
        if (end == std::string::npos)
        {
            cmd.params.push_back(input.substr(pos));
            break;
        }

        cmd.params.push_back(input.substr(pos, end - pos));
        pos = end + 1;
    }

    return cmd;
}

void Bot::sendRaw(const std::string& msg)
{
    std::string out = msg + "\r\n";
    send(_sock, out.c_str(), out.size(), 0);
}

std::string Bot::trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \r\n\t");
    size_t end = s.find_last_not_of(" \r\n\t");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

std::string	Bot::intToString(int n)
{
	std::stringstream	ss;

	ss << n;
	return (ss.str());
}

int	Bot::calculateScore(const std::vector<int>& hand)
{
	int	score = 0;
	int	aces = 0;
	int	val;

	for (size_t i = 0; i < hand.size(); ++i)
	{
		val = hand[i];
		if (val == 1)
		{
			aces++;
			score += 11;
		}
		else if (val > 10)
			score += 10;
		else
			score += val;
	}
	while (score > 21 && aces > 0)
	{
		score -= 10;
		aces--;
	}
	return (score);
}

std::string	Bot::handToString(const std::vector<int>& hand)
{
	std::string	res;
	int			v;

	for (size_t i = 0; i < hand.size(); ++i)
	{
		v = hand[i];
		if (v == 1)
			res += "A ";
		else if (v == 11)
			res += "J ";
		else if (v == 12)
			res += "Q ";
		else if (v == 13)
			res += "K ";
		else
			res += intToString(v) + " ";
	}
	return (res);
}

void	Bot::dispatch(const std::string& name, const Command& cmd)
{
	if (name != "PRIVMSG" || cmd.trailing.empty())
		return ;

	std::string	target = cmd.params.empty() ? _nick : cmd.params[0];
	std::string	user = cmd.prefix.substr(0, cmd.prefix.find('!'));
	GameState	&game = _games[user];
	int			score;

	if (cmd.trailing == "!ping")
		sendRaw("PRIVMSG " + target + " :pong");
	else if (cmd.trailing == "!blackjack")
	{
		srand(time(0));
		game.playerHand.clear();
		game.dealerHand.clear();
		game.playerHand.push_back(rand() % 13 + 1);
		game.playerHand.push_back(rand() % 13 + 1);
		game.dealerHand.push_back(rand() % 13 + 1);
		game.active = true;
		sendRaw("PRIVMSG " + target + " :\x03" "11###########################");
		sendRaw("PRIVMSG " + target + " :\x03" "07#   Welcome to Blackjack  #");
		sendRaw("PRIVMSG " + target + " :\x03" "11###########################");
		sendRaw("PRIVMSG " + target + " :" + user + "Hand: " + handToString(game.playerHand) + "(Score: " + intToString(calculateScore(game.playerHand)) + ") | Dealer: " + handToString(game.dealerHand));
	}
	else if (cmd.trailing == "!hit" && game.active)
	{
		game.playerHand.push_back(rand() % 13 + 1);
		score = calculateScore(game.playerHand);
		sendRaw("PRIVMSG " + target + " :" + user + "Hand: " + handToString(game.playerHand) + "(Score: " + intToString(score) + ")");
		if (score > 21)
		{
			sendRaw("PRIVMSG " + target + " :Bust ! Dealer win.");
			game.active = false;
		}
	}
	else if (cmd.trailing == "!stand" && game.active)
	{
		while (calculateScore(game.dealerHand) < 17)
			game.dealerHand.push_back(rand() % 13 + 1);
		int ps = calculateScore(game.playerHand);
		int ds = calculateScore(game.dealerHand);
		sendRaw("PRIVMSG " + target + " :Dealer Hand: " + handToString(game.dealerHand) + "(Score: " + intToString(ds) + ")");
		if (ds > 21 || ps > ds)
			sendRaw("PRIVMSG " + target + " :" + user + "You win !");
		else if (ds > ps)
			sendRaw("PRIVMSG " + target + " :Dealer win !");
		else
			sendRaw("PRIVMSG " + target + " :Push !");
		game.active = false;
	}
}
