#ifndef NETWORK_H_
#define NETWORK_H_

//-----------------------------------------------------------------------------

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;

#define	GAME_PORT	5225	// default port "EVE"
#define	BUFFER_SIZE	1024

//-----------------------------------------------------------------------------

class Hive : public boost::enable_shared_from_this<Hive>
{
private:
	boost::asio::io_service m_ioService;
	boost::shared_ptr<boost::asio::io_service::work> m_work;
	volatile uint32_t m_shutdown;
public:
	Hive();
	virtual ~Hive();
		
	// Returns the io_service of this object.
	boost::asio::io_service &GetService();

	// Returns true if the Stop function has been called.
	bool HasStopped();

	// Polls the networking subsystem once from the current thread and 
	// returns.
	void Poll();

	// Runs the networking system on the current thread. This function blocks 
	// until the networking system is stopped, so do not call on a single 
	// threaded application with no other means of being able to call Stop 
	// unless you code in such logic.
	void Run();

	// Stops the networking system. All work is finished and no more 
	// networking interactions will be possible afterwards until Reset is called.
	void Stop();

	// Restarts the networking system after Stop as been called. A new work
	// object is created ad the shutdown flag is cleared.
	void Reset();
};

//-----------------------------------------------------------------------------

class Connection : public boost::enable_shared_from_this<Connection>
{
private:
	udp::socket m_socket;
	udp::endpoint m_endpoint;
	std::vector<uint8_t> m_recvBuffer;
	std::list<int32_t> m_pendingRecvs;
	std::list< std::vector<uint8_t> > m_pendingSends;
	int32_t m_recvBufferSize;
	volatile uint32_t m_errorState;
	std::string m_hostAddress;

private:
	void StartReceive(int32_t size = 0);
	void HandleReceive(const boost::system::error_code &error, std::size_t total);

	void StartSend();
	void HandleSend(const boost::system::error_code & error, std::list< std::vector< uint8_t > >::iterator itr);

	void StartError( const boost::system::error_code & error );

	/**
	 * Resolve the IP Address and save it.
	 */
	void ResolveIPAddress(boost::asio::io_service &ioService);
private:
	/**
	 * Called when an error is encountered.
	 */
	virtual void OnError( const boost::system::error_code & error ) = 0;

	/**
	 * Called when data has been sent by the connection.
	 */
	virtual void OnSend(const std::vector<uint8_t> & buffer) = 0;

	/**
	 * Called when data has been received by the connection. 
	 */
	virtual void OnRecv(std::vector<uint8_t> &buffer) = 0;
	
public:
	Connection(boost::asio::io_service &ioService);
	~Connection();

	/**
	 * Posts data to be sent to the connection.
	 */
	void Send(const std::vector<uint8_t> & buffer);

	/**
	 * Posts a recv for the connection to process. If total is 0, then 
	 * as many bytes as possible up to GetReceiveBufferSize() will be 
	 * waited for. If Recv is not 0, then the connection will wait for exactly
	 * total before invoking OnRecv.
	 */
	void Recv(int32_t total = 0);

	/**
	 * Sets the receive buffer size.
	 */
	void SetReceiveBufferSize(int32_t size);

	/**
	 * Gets the receive buffer size.
	 */
	int32_t GetReceiveBufferSize() const;

	/**
	 * Returns true if this object has an error associated with it.
	 */
	bool HasError();

	/**
	 * Returns the IP address of the host.
	 */
	std::string GetHostAddress();
};

#endif
