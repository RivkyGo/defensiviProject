#pragma once
#include <stdint.h>
#include <tuple>
#include <string>

class FileServerRequest
{

private:

    unsigned char* client_id;
    unsigned char version;
    uint16_t code;
    uint32_t payload_size;
    //char* payload;


public:
    FileServerRequest(unsigned char* client_id, unsigned char version, uint16_t code, uint32_t payload_size);//, char* payload);
    //~FileServerRequest();
    std::tuple<const uint8_t*, const uint64_t> GenerateRegisterRequest(std::string);
    std::tuple<const uint8_t*, const uint64_t> GenerateSendingPKRequest(std::string, char*);
    std::tuple<const uint8_t*, const uint64_t> GenerateEncryptedFileSendRequest(size_t file_size, std::string, char*);
    std::tuple<const uint8_t*, const uint64_t> GenerateValidationFileIndication(std::string);

};

