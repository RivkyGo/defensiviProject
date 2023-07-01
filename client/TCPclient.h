#pragma once
#include <string>
#include "client_request.h"
#include "utils.h"
#include "Exeptions.h"

constexpr unsigned int NUMBER_TRIES_TO_SEND = 3;
constexpr unsigned int SIZE_UUID = 16;

class TCPclient
{
	

public:
	void manageClient(std::string);

};

