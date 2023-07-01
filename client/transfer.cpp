#include "transfer.h"

using boost::asio::ip::tcp;


transfer::transfer(std::string IP, std::string Port, std::string name, std::string file_path) : IP(IP), Port(Port), name(name), file_path(file_path) {
}

std::string transfer::getIP() const {
	return IP;
}

std::string transfer::getPort() const {
	return Port;
}

std::string transfer::getName() const {
	return name;
}

std::string transfer::getfile_path() const {
	return file_path;
}



std::string transfer::send_data(const uint8_t* raw_buffer, const uint64_t size)
{
	
	const int max_length = 4096;
	boost::asio::io_context io_context;
	tcp::socket s(io_context);
	tcp::resolver resolver(io_context);
	boost::asio::connect(s, resolver.resolve(IP, Port));
	boost::asio::write(s, boost::asio::buffer(raw_buffer, size));
	boost::array<char, 1028> buffer;
	size_t len = s.read_some(boost::asio::buffer(buffer, max_length));
	std::string response_data(buffer.begin(), len);
	s.close();

	std::cout << "[DEBUG] Recieved response (size:" << std::to_string(len) <<" )"  << std::endl;

	return response_data;

	

};
