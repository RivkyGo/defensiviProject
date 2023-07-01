#include "FileServerRequest.h"
#include <boost/any.hpp>


FileServerRequest::FileServerRequest(unsigned char* client_id, unsigned char version, uint16_t code, uint32_t payload_size) : client_id(client_id), version(version), code(code), payload_size(payload_size) {}

//FileServerRequest::~FileServerRequest()
//{
//	delete [] client_id;
//}



std::tuple<const uint8_t*, const uint64_t> FileServerRequest::GenerateRegisterRequest(std::string name)
{

#pragma pack(push, 1)

	struct RawRequest {
		unsigned char client_id[16];
		unsigned char version;
		uint16_t code;
		uint32_t payload_size;
	};

	struct RegisterRequest
	{
		RawRequest raw_req;
		char payload[255];
	};


	struct RegisterRequest req;

	name = name + '\0';
	std::memcpy(req.raw_req.client_id, client_id, 16);
	req.raw_req.version = version;
	req.raw_req.code = code;
	req.raw_req.payload_size = payload_size;

	std::memcpy(req.payload, name.c_str(), payload_size);


	const uint8_t* temp_buffer = reinterpret_cast<const uint8_t*>(&req);

	uint8_t* raw_buffer = new uint8_t[sizeof(req)];

	std::memcpy(raw_buffer, temp_buffer, sizeof(req));

	const uint8_t* final_buffer = raw_buffer;

	return std::make_tuple(final_buffer, sizeof(req));

#pragma pack(pop)
}




std::tuple<const uint8_t*, const uint64_t> FileServerRequest::GenerateSendingPKRequest(std::string username, char* pk)
{

#pragma pack(push, 1)

	struct RawRequest {
		unsigned char client_id[16];
		unsigned char version;
		uint16_t code;
		uint32_t payload_size;
	};

	struct PublicKeySendRequest
	{
		RawRequest raw_req;
		char username[255];
		char payload[160];
	};

	struct PublicKeySendRequest req;


	username = username + '\0';
	//pk = pk + '\0';
	std::memcpy(req.raw_req.client_id, client_id, 16);
	req.raw_req.version = version;
	req.raw_req.code = code;
	req.raw_req.payload_size = sizeof(req) - sizeof(req.raw_req);

	std::memcpy(req.username, username.c_str(), username.length());
	std::memcpy(req.payload, pk, payload_size);


	const uint8_t* temp_buffer = reinterpret_cast<const uint8_t*>(&req);

	uint8_t* raw_buffer = new uint8_t[sizeof(req)];

	std::memcpy(raw_buffer, temp_buffer, sizeof(req));

	const uint8_t* final_buffer = raw_buffer;

	return std::make_tuple(final_buffer, sizeof(req));

#pragma pack(pop)
}

std::tuple<const uint8_t*, const uint64_t> FileServerRequest::GenerateEncryptedFileSendRequest(size_t file_size, std::string file_name, char* encrypted_file_content)
{
#	pragma pack(push, 1)

	struct RawRequest {
		unsigned char client_id[16];
		unsigned char version;
		uint16_t code;
		uint32_t payload_size;
	};

	struct SendAESEncryptedFile
	{
		RawRequest raw_req;
		uint32_t file_size;
		char file_name[255];
		//char* message_content;
	};

	struct SendAESEncryptedFile req;

	
	std::memcpy(req.raw_req.client_id, client_id, 16);
	req.raw_req.version = version;
	req.raw_req.code = code;
	req.file_size = file_size;

	
	std::memcpy(req.file_name, file_name.c_str(), 255);
	req.raw_req.payload_size = sizeof(req.file_size) + sizeof(req.file_name) + req.file_size;

	const uint8_t* temp_buffer = reinterpret_cast<const uint8_t*>(&req);

	size_t total_buffer_size = sizeof(req) + req.raw_req.payload_size;
	uint8_t* raw_buffer = new uint8_t[total_buffer_size];

	std::memcpy(raw_buffer, temp_buffer, sizeof(req));
	std::memcpy(raw_buffer + sizeof(req), encrypted_file_content, file_size);

	const uint8_t* final_buffer = raw_buffer;
	
	return std::make_tuple(final_buffer, total_buffer_size);
	
	

#pragma pack(pop)
}


std::tuple<const uint8_t*, const uint64_t> FileServerRequest::GenerateValidationFileIndication(std::string file_name)
{
#	pragma pack(push, 1)

	struct RawRequest {
		unsigned char client_id[16];
		unsigned char version;
		uint16_t code;
		uint32_t payload_size;
	};

	struct ValidFileRequest
	{
		RawRequest raw_req;
		char file_name[255];
	};

	struct ValidFileRequest req;

	file_name = file_name + '\0';
	std::memcpy(req.raw_req.client_id, client_id, 16);
	req.raw_req.version = version;
	req.raw_req.code = code;
	

	req.raw_req.payload_size =   sizeof(req.file_name);

	std::memcpy(req.file_name, file_name.c_str(), file_name.length());


	const uint8_t* temp_buffer = reinterpret_cast<const uint8_t*>(&req);

	uint8_t* raw_buffer = new uint8_t[sizeof(req)];

	std::memcpy(raw_buffer, temp_buffer, sizeof(req));

	const uint8_t* final_buffer = raw_buffer;

	return std::make_tuple(final_buffer, sizeof(req));


#pragma pack(pop)
}

