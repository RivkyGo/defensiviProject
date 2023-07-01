#pragma once
#include "transfer.h"
#include "utils.h"
#include "FileServerRequest.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Exeptions.h"
#include "Base64Wrapper.h"
#include <iostream>
#include <utility>

constexpr uint16_t REQUEST_SEND_PK = 1101;
constexpr unsigned int REQUEST_SEND_FILE = 1103;
constexpr unsigned int REQUEST_REGISTER_CODE = 1100;
constexpr unsigned int REQUEST_VALID_CRC = 1104;
constexpr unsigned int REQUEST_INVALID_CRC = 1105;
constexpr unsigned int REQUEST_INVALID_CRC_TERMINATING = 1106;
constexpr unsigned int REQUEST_RECONECTING_CODE = 1102;

class client_request
{
	static const unsigned char CLIENT_VERSION = 3;
	static const int PAYLOAD_SIZE = 255;
	static const int RESPONSE_FAILD = 7;
	static const int RESPONSE_HEADERS = 7;
	static const int RESPONSE_FILE_HEADERS_TOTAL_SIZE = 282;
	
	
public:

	static transfer InitClient();

	static unsigned char* RegistrationReq(transfer tcp_client, std::string username);
	static void RequestToReconnect(transfer tcp_client, std::string& encrypted_aes_key, unsigned char* & client_id);
	static std::vector<std::string> SendPK(transfer tcp_client, unsigned char* client_id, std::string username);
	static std::string DecryptAESKey(std::string encrypted_aes_key, std::string private_key);
	static std::string EncryptFileAES(std::string aes_key, std::string file_data);
	static bool SendEncryptedFile(transfer , unsigned char* , std::string );
	static void SendValidCrc(transfer , unsigned char* );
	static void SendInvalidCRC(transfer , unsigned char* );
	static void SendTerminatingSessionInvalidCRC(transfer , unsigned char*);

};

