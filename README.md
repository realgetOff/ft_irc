*This project has been created as part of the 42 curriculum by <mforest->,<lviravon>.*

# ft_irc

## Description
**ft_irc** is a custom Internet Relay Chat (IRC) server written in **C++98**. The project aims to recreate a functional chat server capable of handling multiple simultaneous connections without crashing or blocking. It implements a non-blocking network architecture using I/O multiplexing with `poll()`, allowing it to serve multiple clients across various channels in real-time.

The server is compatible with official IRC clients (like HexChat, AdiIRC, or Irssi) and follows the communication standards defined in **RFC 2812**.

## Features
- **Authentication**: Secure connection using a server password and nickname/username registration.
- **Channel Management**: Create, join, and leave multiple channels simultaneously.
- **Operator Privileges**: KICK, INVITE, TOPIC, and MODE commands to manage users and channel settings.
- **Channel Modes**:
    - `i`: Invite-only channel.
    - `t`: Topic restrictions for operators.
    - `k`: Channel key (password).
    - `o`: Give/take channel operator privilege.
    - `l`: Set user limit for the channel.
- **Private Messaging**: Direct messages between users and notices.
- **Blackjack Bot**: A fully integrated game bot with IRC color formatting and score tracking.

## Instructions

### Compilation
The project uses a Makefile and must be compiled on a Unix-based system.
```bash
make

```

### Execution

Run the server by specifying a port (usually 6667) and a password for client connections.

```bash
./ircserv <port> <password>

```

### Usage

1. Open an IRC client (e.g., **HexChat**).
2. Create a new network with the address `127.0.0.1` and the chosen port.
3. Enter the password you used to launch the server.
4. Join a channel using `/join #chan_name`.

## Resources

* RFC 2812: The official Internet Relay Chat: Client Protocol.
* RFC 1459: Original IRC Protocol specifications.
* geeksforgeeks.org Socket Programming in C++: Essential resource for socket programming in C++.

### Use of AI

* **Protocol Debugging**: Resolving issues with GUI clients (HexChat) by identifying missing mandatory numerical replies (`RPL_NAMREPLY`, `RPL_ENDOFNAMES`).

## Technical Choices

* **Multiplexing**: Utilized `poll()` for managing multiple file descriptors to ensure high responsiveness and efficiency.
* **Non-blocking Sockets**: All client sockets are set to `O_NONBLOCK` to prevent the server from hanging during slow read/write operations.
* **STL Containers**: Heavy use of `std::map` and `std::vector` to efficiently manage the relationships between clients and channels.
# ft_irc
