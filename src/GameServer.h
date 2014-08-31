#ifndef GAME_SERVER_H__
#define GAME_SERVER_H__

#include "Network.h"
#include "PlayerController.h"
#include "Settings.h"

#include <string>
#include <ctime>

/**
 * Protocol:
 *  Header - 2 bytes
 *		[0] = type, HELLO (c/s), SERVER_INFO (s), NEW (c), VERIFY (s), CONTROL (c), QUIT (c/s), START (c/s), FULL (s), PING (c/s), PONG (c/s)
 *		[1] = client id, server = 0, player 1 = 1, ...
 *
 *	 HELLO - Sent by the client to search for servers, servers respond with a SERVER_INFO message.
 *	 MESSAGE_SERVERINFO - Send server name to the client.
 *	 NEW - Send by the client to identify itself with a name, server responds with a VERIFY message including the ID of the client.
 *	 VERIFY - Sent by the server to set an ID to the client sending a NEW message.
 *	 CONTROL - Sent by the client to inform new data input, the control data is send as the first byte in the message.
 *	 QUIT - The client sends a QUIT message to bounce the queue; the server sends this command to stop the game.
 *	 START - The master client can send this command to remotely start the game. Server respond with a STATUS message.
 *	 FULL - Sent by the server as a respons to the NEW command if the server is full.
 *	 PING - Sent to check for liveliness
*	 PONG - I'M ALIVE
 *	
 *		Client:
 *			<name>	// client identify itself with an id
 *			[0]		// control data (left, right or none)
 *		Server:
 *			OK		// Aight! Header contains the id of the client
 */
enum MESSAGE_TYPES { MESSAGE_HELLO = 0, MESSAGE_SERVERINFO, MESSAGE_NEW, MESSAGE_VERIFY, MESSAGE_CONTROL, MESSAGE_QUIT, MESSAGE_START, MESSAGE_FULL, MESSAGE_PING, MESSAGE_PONG };

// server states
enum SERVER_STATES { SERVER_ACCEPT = 0, SERVER_REQUEST_START, SERVER_GAME_STARTED, SERVER_ENDED };

class GameServer : public Connection
{
private:
	std::vector<PlayerController> m_players;	// map player controllers to random id's used in the verification process
	std::string m_name;
	int m_serverState;

	/**
	 * Translate the message and process it.
	 */
	void TranslateMessage(std::vector<uint8_t> buffer);
	
	/**
	 * Send OK message to the player containing:
	 *	player	the player number (0 ... 5)
	 *	uid		the unique identifier needed for the client to send messages.
	 */
	void SendOK(int role, int uid);

	/**
	 * Send ServerInfo message containing the server name.
	 */
	void SendServerInfo(std::string name);

	/**
	 * Send FULL message in a response to a NEW command on a full server.
	 */
	void SendFull();

	/**
	 * Send Pong in response to Ping.
	 */
	void SendPong();

	/**
	 * Find the player with <id> and return the index.
	 * @return index of the player or -1 when the player is not found.
	 */
	int FindPlayer(int id);
public:
	GameServer(boost::asio::io_service &ioService, std::string name);
	~GameServer();

	std::vector<PlayerController> *GetPlayerControllers();
	void FlushPlayerControllers();

	/**
	 * Return the name of the server.
	 */
	std::string GetServerName();

	/**
	 * Called when data has been sent by the connection.
	 */
	void OnSend(const std::vector<uint8_t> &buffer);

	/**
	 * Called when data has been received by the connection. 
	 */
	void OnRecv(std::vector<uint8_t> &buffer);

	/**
	 * Called when an error is encountered.
	 */
	void OnError(const boost::system::error_code & error);

	/**
	 * Set the state of the server.
	 */
	void SetServerState(int state);

	/**
	 * Get the state of the server.
	 */
	int GetServerState();

	/**
	 * Clear all the connections so new connections can join the game.
	 */
	void ClearServer();
};

#endif