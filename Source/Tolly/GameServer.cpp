#include "GameServer.h"

GameServer::GameServer(boost::asio::io_service &ioService, std::string name) : Connection(ioService), m_serverState(SERVER_ACCEPT)
{
	m_name = name;
	Recv();
}

GameServer::~GameServer()
{
}

/**
 * Called when data has been sent by the connection.
 */
void GameServer::OnSend(const std::vector<uint8_t> & buffer)
{
}

/** 
 * Called when data has been received by the connection. 
 */
void GameServer::OnRecv(std::vector<uint8_t> &buffer)
{
	// message received from client, check if this is a new one or data from an existing one
	TranslateMessage(buffer);

	std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
	for( size_t x = 0; x < buffer.size(); ++x )
	{
		std::cout << buffer[x];
	}
	std::cout << std::endl;

	// Start the next receive
	Recv();
}

/**
 * Called when an error is encountered.
 */
void GameServer::OnError(const boost::system::error_code & error)
{
}

void GameServer::SetServerState(int state)
{
	m_serverState = state;
}

int GameServer::GetServerState()
{
	return m_serverState;
}

/**
 * Translate the message and process it.
 */
void GameServer::TranslateMessage(std::vector<uint8_t> buffer)
{
	srand( static_cast<unsigned int>(time(0)));

	if(buffer.size() < 2)
		return;

	// check message type
	if(buffer[0] == MESSAGE_NEW)
	{
		std::cout << "[DEBUG]: New message received" << std::endl;
		if((int)m_players.size() >= MAX_PLAYERS || (m_serverState != SERVER_ACCEPT))
		{
			SendFull();
		}
		else
		{
			// new client, create id
			PlayerController controller;

			// strip payload == name
			std::string name;
			for(int i = 2; i < (int)buffer.size(); i++)
				name += (char)buffer[i];	// convert to ASCII

			// create random identifier between 0 and 128
			int id = abs(rand() % SCHAR_MAX);

			// don't allow names with more than 8 characters
			name = name.substr(0, 8);
			controller.name = name;
			controller.id = id;
			m_players.push_back(controller);

			// send OK
			SendOK(m_players.size()-1, id);
		}
	}
	else if(buffer[0] == MESSAGE_CONTROL)
	{
		// check payload
		std::cout << "[DEBUG]: Control message received" << std::endl;

		int id = buffer[1];
		int index = FindPlayer(id);
		if(index < 0)
			return;

		m_players[index].input = buffer[2];
	}
	else if(buffer[0] == MESSAGE_START)
	{
		std::cout << "[DEBUG]: Start message received" << std::endl;

		// start the game if there are enough players
		if(m_serverState == SERVER_ACCEPT)
			m_serverState = SERVER_REQUEST_START;
		// restart the game if the game was ended
		else if(m_serverState == SERVER_ENDED)
			m_serverState = SERVER_ACCEPT;
	}
	else if(buffer[0] == MESSAGE_QUIT)
	{
		std::cout << "[DEBUG]: Quit message received" << std::endl;

		if(m_serverState == SERVER_ACCEPT || m_serverState == SERVER_ENDED)
		{
			int id = buffer[1];
			int index = FindPlayer(id);
			if(index < 0)
				return;

			std::cout << "[DEBUG]: Removing player " << id << std::endl;
			m_players.erase(m_players.begin() + index);
		}
	}
	else if(buffer[0] == MESSAGE_HELLO)
	{
		// Is it me you are looking for!
		std::cout << "[DEBUG]: HELLO message received!" << std::endl;
		if(m_serverState == SERVER_ACCEPT)
			SendServerInfo(m_name);
	}
	else
	{
		std::cout << "[DEBUG]: Unknown message received" << std::endl;
	}
}

/**
 * Send OK message to the player containing:
 *	player	the player number (0 ... 5)
 *	uid		the unique identifier needed for the client to send messages.
 */
void GameServer::SendOK(int player, int uid)
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_VERIFY);
	data.push_back((uint8_t)0);	// server id
	data.push_back((uint8_t)player);
	data.push_back((uint8_t)uid);
	Send(data);
}

/**
 * Send FULL message in a respons to a NEW command on a full server.
 */
void GameServer::SendFull()
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_FULL);
	data.push_back((uint8_t)0);
	std::string message = "FULL";
	std::copy(message.begin(), message.end(), back_inserter(data));
	Send(data);
}

/**
 * Send ServerInfo message to identify as a server.
 * @param	name	name of the server
 */
void GameServer::SendServerInfo(std::string name)
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_SERVERINFO);
	data.push_back((uint8_t)0);
	std::string message = name;
	std::copy(message.begin(), message.end(), back_inserter(data));
	Send(data);
}

/**
 * Return the player controllers.
 */
std::vector<PlayerController> *GameServer::GetPlayerControllers()
{
	return &m_players;
}

/**
 * Clear player input.
 */
void GameServer::FlushPlayerControllers()
{
	for(int i = 0; i < (int)m_players.size(); i++)
		m_players[i].input = NONE;
}

/**
 * Get the server name.
 */
std::string GameServer::GetServerName()
{
	return m_name;
}

/**
 * Find the player with <id> and return the index.
 * @return index of the player or -1 when the player is not found.
 */
int GameServer::FindPlayer(int id)
{
	for(int i = 0; i < (int)m_players.size(); i++)
	{
		if(m_players[i].id == id)
			return i;
	}
	return -1;
}

/**
 * Clear all the connections so new connections can join the game.
 */
void GameServer::ClearServer()
{
	m_players.clear();
	SetServerState(SERVER_ACCEPT);
}