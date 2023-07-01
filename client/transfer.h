#pragma once
#include <string.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class transfer
{
	std::string IP;
	std::string Port;
	std::string name;
	std::string file_path;

public:
	transfer(std::string IP, std::string Port, std::string name, std::string file_path);
	std::string getIP() const;
	std::string getPort() const;
	std::string getName() const;
	std::string getfile_path() const;
	std::string send_data(const uint8_t* raw_buffer, const uint64_t size);
};

