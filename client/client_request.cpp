#include "client_request.h"




transfer client_request::InitClient()
{
	utils u;
	std::string* buffer = u.readFile();
	transfer TCPclient(buffer[0], buffer[1], buffer[2], buffer[3]);
	
	return TCPclient;
}

unsigned char* client_request::RegistrationReq(transfer tcp_client, std::string username) {
	try
	{
		utils u;
		std::cout << "Starting with registering the user." << std::endl;
		unsigned char* fake_client_id = new unsigned char[16];
		u.random_client_id(fake_client_id, 16);

		std::string str = tcp_client.getName();
		char* payload = new char[str.length() + 1];
		u.add_null_terminating_byte(str, payload);

		FileServerRequest req(fake_client_id, CLIENT_VERSION, REQUEST_REGISTER_CODE, PAYLOAD_SIZE);
		std::tuple<const uint8_t*, const uint64_t> generateRequst = req.GenerateRegisterRequest(tcp_client.getName());
		auto buff = std::get<0>(generateRequst);
		auto buff_size = std::get<1>(generateRequst);

		auto response = tcp_client.send_data(buff, buff_size);

		if (response.length() == RESPONSE_FAILD) {
			throw ExceptionRegistrationFailed(std::exception()); // The name already exists in the system
		}
			
		unsigned char* client_id = new unsigned char[16];
		std::memcpy(client_id, response.c_str() + RESPONSE_HEADERS, 16);

		delete buff;
		delete payload;

		return client_id;
	}
	catch (const ExceptionUserRegistered & ex)
	{
		throw ExceptionUserRegistered(ex);  
	}
}



void client_request::RequestToReconnect(transfer tcp_client, std::string & encrypted_aes_key, unsigned char *& client_id) {
	//read the uuid from me.info 
	std::string uuid_in_ascii = utils::readMeFile(0); //to read line 1
	std::string uuid_in_hex = utils::decode_uuid(uuid_in_ascii);
	const char* uuid1 = uuid_in_hex.c_str();
	unsigned char* uuid = reinterpret_cast<unsigned char*>(const_cast<char*>(uuid1));

	FileServerRequest req(uuid, CLIENT_VERSION, REQUEST_RECONECTING_CODE, PAYLOAD_SIZE);
	std::tuple<const uint8_t*, const uint64_t> generateRequst = req.GenerateRegisterRequest(tcp_client.getName());// check if to put payload instead getName

	auto buff = std::get<0>(generateRequst);
	auto buff_size = std::get<1>(generateRequst);

	auto response = tcp_client.send_data(buff, buff_size);

	if (response.length() == RESPONSE_FAILD) {
	
		throw ExceptionReconnecting(std::exception());

	}

	int client_int_start = RESPONSE_HEADERS;
	int client_int_end = RESPONSE_HEADERS + 16;
	int aes_key_start = RESPONSE_HEADERS + 16;
	int aes_key_end = response.length() - aes_key_start;

	auto _client_id = response.substr(client_int_start, client_int_end - client_int_start);
	encrypted_aes_key = response.substr(aes_key_start, aes_key_end);
	
	std::memcpy(client_id, uuid, 16);
	client_id[16] = '\0';
	
}







std::vector<std::string> client_request::SendPK(transfer tcp_client, unsigned char* client_id, std::string username) {

	try
	{
		std::cout << "[INFO] Creating a public and private key and sending it to the server" << std::endl;
		RSAPrivateWrapper rsaprivate;
		std::string pubkey = rsaprivate.getPublicKey();
		std::string privatekey = rsaprivate.getPrivateKey();
		std::string base64key = Base64Wrapper::encode(privatekey);
		utils::writeToFile( "me.info.txt", base64key);
		//write the private key encryped_and_pk.fromt() in me.info

 		char* raw_pubkey = new char[pubkey.length()];
		std::memcpy(raw_pubkey, pubkey.c_str(), pubkey.length());

		FileServerRequest req(client_id, CLIENT_VERSION, REQUEST_SEND_PK, pubkey.length()); //+ PAYLOAD_SIZE
		std::tuple<const uint8_t*, const uint64_t> raw_request = req.GenerateSendingPKRequest(tcp_client.getName(), raw_pubkey);

		auto buffer = std::get<0>(raw_request);
		auto buffer_size = std::get<1>(raw_request);

		auto response = tcp_client.send_data(buffer, buffer_size);

		int client_int_start = RESPONSE_HEADERS;
		int client_int_end = RESPONSE_HEADERS + 16;
		int aes_key_start = RESPONSE_HEADERS + 16;
		int aes_key_end = response.length() - aes_key_start;

		auto _client_id = response.substr(client_int_start, client_int_end - client_int_start);
		auto encrypted_aes_key = response.substr(aes_key_start, aes_key_end);

		std::vector<std::string> packed_encryped_and_pk = { privatekey , encrypted_aes_key };

		delete buffer;
		return packed_encryped_and_pk;
	}
	catch (const std::exception& ex)
	{
		//throw ExceptionSendRSAPublicKey(ex);
	}
}


std::string client_request::DecryptAESKey(std::string encrypted_aes_key, std::string private_key)
{
	std::cout << "Open encryption using the private key \n" << std::endl;
	try {
		RSAPrivateWrapper rsapriv_other(private_key);

		std::string decrypted = rsapriv_other.decrypt(encrypted_aes_key);

		return decrypted;
	}
	catch (const std::exception& ex)
	{
		throw ExceptionDecryptAESKey(ex);
		
	}
}

#include <iostream>
#include <iomanip>
void hexify(const unsigned char* buffer, unsigned int length)
{

	std::cout << "aes key in hex \n" << std::endl;
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}

std::string client_request::EncryptFileAES(std::string aes_key, std::string file_data)
{
	try {
		std::cout << "[INFO] Encrypting the file with received AES key" << std::endl;
		// 1. Generate a key and initialize an AESWrapper. You can also create AESWrapper with default constructor which will automatically generates a random key.
		unsigned char* key = new unsigned char[AESWrapper::DEFAULT_KEYLENGTH];
		std::memcpy(key, aes_key.c_str(), AESWrapper::DEFAULT_KEYLENGTH);

		hexify(key, AESWrapper::DEFAULT_KEYLENGTH);

		AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);

		// 2. encrypt a message (plain text)
		std::string ciphertext = aes.encrypt(file_data.c_str(), file_data.length());

		return ciphertext;
	}
	catch (std::exception& e)
	{
		//throw ExceptionSendingEncryptedFile(e);
	}
}

bool client_request::SendEncryptedFile(transfer tcp_client, unsigned char* client_id, std::string ciphertext) {
	try
	{
		std::string file_path = tcp_client.getfile_path();
		std::string base_filename = file_path.substr(file_path.find_last_of("/\\") + 1);

		char* raw_payload = new char[ciphertext.length()];
		std::memcpy(raw_payload, ciphertext.c_str(), ciphertext.length());

		FileServerRequest req(client_id, CLIENT_VERSION, REQUEST_SEND_FILE, ciphertext.length());

		std::tuple<const uint8_t*, const uint64_t> raw_request = req.GenerateEncryptedFileSendRequest(ciphertext.length(),base_filename, raw_payload);

		auto buffer = std::get<0>(raw_request);
		auto buffer_size = std::get<1>(raw_request);

		std::string fileContent = utils::readFileClient(tcp_client.getfile_path());

		uint32_t file_crc = utils::get_crc32(fileContent);

		auto response = tcp_client.send_data(buffer, buffer_size);

		int payload_start_index = RESPONSE_FILE_HEADERS_TOTAL_SIZE;
		int payload_length = response.length() - payload_start_index;
		
		auto crc_pointer = response.c_str() + payload_start_index;

		uint32_t data;
		std::memcpy((char*)&data, crc_pointer, sizeof(data));

		delete raw_payload;
		return file_crc == data;


	}
	catch (const std::exception&)
	{
		//throw ExceptionSendingEncryptedFile(e);
	}
}


void client_request::SendValidCrc(transfer tcp_client, unsigned char* client_id)
{
	try
	{
		std::cout << "[INFO] The file was recieved at the server valid" << std::endl;

		std::string file_path = tcp_client.getfile_path();
		std::string base_filename = file_path.substr(file_path.find_last_of("/\\") + 1);

		FileServerRequest req(client_id, CLIENT_VERSION, REQUEST_VALID_CRC, base_filename.length());
		std::tuple<const uint8_t*, const uint64_t> raw_request = req.GenerateValidationFileIndication(base_filename);
		auto buffer = std::get<0>(raw_request);
		auto buffer_size = std::get<1>(raw_request);

		tcp_client.send_data(buffer, buffer_size);
	}
	catch (std::exception& e)
	{
		//throw ExceptionSendingFileIndication(e);
	}
}


void client_request::SendInvalidCRC(transfer tcp_client, unsigned char* client_id)
{
	try {

		std::cout << "[ERROR] The server recieved invalid file (CRC is not correct)" << std::endl;

		std::string file_path = tcp_client.getfile_path();
		std::string base_filename = file_path.substr(file_path.find_last_of("/\\") + 1);

		FileServerRequest req(client_id, CLIENT_VERSION, REQUEST_INVALID_CRC, base_filename.length());
		std::tuple<const uint8_t*, const uint64_t> raw_request = req.GenerateValidationFileIndication(base_filename);
		auto buffer = std::get<0>(raw_request);
		auto buffer_size = std::get<1>(raw_request);

		tcp_client.send_data(buffer, buffer_size);

	}

	catch (std::exception& e)
	{
		//throw ExceptionSendingFileIndication(e);
	}

}
void client_request::SendTerminatingSessionInvalidCRC(transfer tcp_client, unsigned char* client_id)
{
	try {
		std::cout << "[CRITICAL] Terminating session, file is invalid after 3 tries." << std::endl;

		std::string file_path = tcp_client.getfile_path();
		std::string base_filename = file_path.substr(file_path.find_last_of("/\\") + 1);

		FileServerRequest req(client_id, CLIENT_VERSION, REQUEST_INVALID_CRC_TERMINATING, base_filename.length());
		std::tuple<const uint8_t*, const uint64_t> raw_request = req.GenerateValidationFileIndication(base_filename);
		auto buffer = std::get<0>(raw_request);
		auto buffer_size = std::get<1>(raw_request);

		tcp_client.send_data(buffer, buffer_size);
	}


	catch (std::exception& e)
	{
		//throw ExceptionSendingFileIndication(e);
	}
}




