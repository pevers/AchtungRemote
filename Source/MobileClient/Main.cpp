#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <ctime>

using boost::asio::ip::udp;

#define	GAME_PORT	"5225"
#define GAME_HOST	"localhost"

enum MESSAGE_TYPES { MESSAGE_HELLO = 0, MESSAGE_SERVERINFO, MESSAGE_NEW, MESSAGE_VERIFY, MESSAGE_CONTROL, MESSAGE_QUIT, MESSAGE_START, MESSAGE_FULL };

/**
 * Removes the header and gives back the payload.
 */
std::string extractData(std::vector<uint8_t> data)
{
	std::string ret;
	for(int i = 2; i < data.size(); i++)
		ret += (char)data[i];	// convert to ASCII
	return ret;
}

std::vector<uint8_t> createNewMessage(std::string name)
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_NEW);
	data.push_back(0);
	std::string tmp = name;
	std::copy(tmp.begin(), tmp.end(), back_inserter(data));
	return data;
}

/**
 * Create a random control message.
 */
std::vector<uint8_t> createRandomControlMessage(int id)
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_CONTROL);
	data.push_back(id);
	data.push_back(rand() % 3);
	return data;
}

std::vector<uint8_t> createQuitMessage(int id)
{
	std::vector<uint8_t> data;
	data.push_back(MESSAGE_QUIT);
	data.push_back((uint8_t)id);
	return data;
}

int main()
{
	srand((unsigned)time(0));
	try
	{
		std::cout << "Press any key to join ... " << std::endl;
		std::cin.get();

		boost::asio::io_service ioService;
		boost::asio::io_service::work work(ioService);
	
		udp::resolver resolver(ioService);
		udp::resolver::query query(udp::v4(), GAME_HOST, GAME_PORT);
		udp::endpoint receiver = *resolver.resolve(query);
		udp::socket socket(ioService);
		socket.open(udp::v4());

		// send data
		std::vector<uint8_t> data;
		socket.send_to(boost::asio::buffer(createNewMessage("TestBot")), receiver);

		std::vector<uint8_t> recv;
		udp::endpoint sender;
		recv.resize(4096);
		size_t len = socket.receive_from(boost::asio::buffer(recv), sender);
		recv.resize(len);

		if(recv.empty())
		{
			std::cerr << "Error while trying to receive data!" << std::endl;
			exit(-1);
		}

		int id = (int)recv[1];
		std::cout << "ID: " << id << std::endl;
		std::cout << "Payload: " << extractData(recv) << std::endl;

		std::cout << "Press [a] to quit, press [b] to simulate input" << std::endl;
		char key = std::cin.get();
		if(key == 'a')
		{
			socket.send_to(boost::asio::buffer(createQuitMessage(id)), receiver);
		} 
		else if(key == 'b')
		{
			for(int i = 0; i < 100; i++)
			{
				socket.send_to(boost::asio::buffer(createRandomControlMessage(id)), receiver);
				boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			}
		}
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}