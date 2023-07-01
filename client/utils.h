#pragma once
#include <fstream>
#include <boost/filesystem.hpp>
#include <iostream>
#include <boost/crc.hpp>
#include <iomanip>


class utils
{
public:
	std::string* readFile();
	static std::string readMeFile(int);
	static std::string decode_uuid(std::string uuid);

	std::string* readFile(std::string);
	static void writeToFile(std::string, std::string);
	static bool exist_meInfo();
	static void uuid_to_hex(const unsigned char* , unsigned int );
	void random_client_id(unsigned char* buf, int size);
	void add_null_terminating_byte(std::string str, char* buf);
	static std::string readFileClient(std::string);
	static uint32_t get_crc32(const std::string& my_string);


};


