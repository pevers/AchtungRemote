#include "Network.h"

//-----------------------------------------------------------------------------

Hive::Hive()
: m_work(new boost::asio::io_service::work(m_ioService)), m_shutdown( 0 )
{
}

Hive::~Hive()
{
}

boost::asio::io_service & Hive::GetService()
{
	return m_ioService;
}

bool Hive::HasStopped()
{
	return ( boost::interprocess::ipcdetail::atomic_cas32( &m_shutdown, 1, 1 ) == 1 );
}

void Hive::Poll()
{
	m_ioService.poll();
}

void Hive::Run()
{
	m_ioService.run();
}

void Hive::Stop()
{
	if( boost::interprocess::ipcdetail::atomic_cas32( &m_shutdown, 1, 0 ) == 0 )
	{
		m_work.reset();
		m_ioService.run();
		m_ioService.stop();
	}
}

void Hive::Reset()
{
	if( boost::interprocess::ipcdetail::atomic_cas32( &m_shutdown, 0, 1 ) == 1 )
	{
		m_ioService.reset();
		m_work.reset( new boost::asio::io_service::work( m_ioService ) );
	}
}

//-----------------------------------------------------------------------------

Connection::Connection(boost::asio::io_service &ioService)
	: m_socket(ioService, udp::endpoint(udp::v4(), GAME_PORT)), m_recvBufferSize(4096), m_errorState(0), m_hostAddress("")
{
	ResolveIPAddress(ioService);
}
Connection::~Connection()
{
}

/**
 * Resolve the IP Address and save it.
 */
void Connection::ResolveIPAddress(boost::asio::io_service &ioService)
{
	udp::resolver resolver(ioService);
	udp::resolver::query query(boost::asio::ip::host_name(), "", boost::asio::ip::resolver_query_base::flags());
	udp::resolver::iterator iter = resolver.resolve(query);
	udp::resolver::iterator end;
	while(iter != end)
	{
		udp::endpoint ep = *iter++;
		if(ep.address().is_v4())
			m_hostAddress = ep.address().to_string();
	}
}

/**
* Returns the IP address of the host.
*/
std::string Connection::GetHostAddress()
{
	return m_hostAddress;
}

/**
 * Posts a recv for the connection to process. If total is 0, then 
 * as many bytes as possible up to GetReceiveBufferSize() will be 
 * waited for. If Recv is not 0, then the connection will wait for exactly
 * total before invoking OnRecv.
 */
void Connection::Recv(int32_t total)
{
	bool shouldStart = m_pendingRecvs.empty();
	m_pendingRecvs.push_back(total);
	if(shouldStart)
	{
		StartReceive(total);
	}
}

/**
 * Posts data to be sent to the connection.
 */
void Connection::Send(const std::vector<uint8_t> &buffer)
{
	bool shouldStart = m_pendingSends.empty();
	m_pendingSends.push_back(buffer);
	if(shouldStart)
	{
		StartSend();
	}
}

/**
 * Receiving data from client.
 */
void Connection::StartReceive(int32_t size)
{
	if(size > 0)
		m_recvBuffer.resize(size);
	else
		m_recvBuffer.resize(m_recvBufferSize);

	m_socket.async_receive_from(
		boost::asio::buffer(m_recvBuffer), m_endpoint,
		boost::bind(&Connection::HandleReceive, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

/**
 * Start sending data to the host.
 */
void Connection::StartSend()
{
	if(!m_pendingSends.empty())
	{
		m_socket.async_send_to(boost::asio::buffer(m_pendingSends.front()), m_endpoint, boost::bind(&Connection::HandleSend, shared_from_this(), boost::asio::placeholders::error, m_pendingSends.begin()));
	}
}

void Connection::HandleReceive(const boost::system::error_code &error, std::size_t size)
{
	if(!error)
	{
		m_recvBuffer.resize(size);

		OnRecv(m_recvBuffer);

		m_pendingRecvs.pop_front();
		if(!m_pendingRecvs.empty())
		{
			StartReceive(m_pendingRecvs.front() );
		}
	}
	else
	{
		StartError(error);
	}
}

void Connection::HandleSend(const boost::system::error_code & error, std::list< std::vector< uint8_t > >::iterator itr)
{
	if(error || HasError())
	{
		StartError(error);
	}
	else
	{
		OnSend(*itr);
		m_pendingSends.erase(itr);
		StartSend();
	}	
}

/**
 * Sets the receive buffer size.
 */
void Connection::SetReceiveBufferSize(int32_t size)
{
	m_recvBufferSize = size;
}

/**
 * Gets the receive buffer size.
 */
int32_t Connection::GetReceiveBufferSize() const
{
	return m_recvBufferSize;
}

bool Connection::HasError()
{
	return ( boost::interprocess::ipcdetail::atomic_cas32(&m_errorState, 1, 1) == 1);
}

void Connection::StartError(const boost::system::error_code &error)
{
	if( boost::interprocess::ipcdetail::atomic_cas32(&m_errorState, 1, 0) == 0)
	{
		boost::system::error_code ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_socket.close(ec);
		OnError(error);
	}
}