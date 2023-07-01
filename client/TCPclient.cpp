#include "TCPclient.h"



void TCPclient::manageClient(std::string user) {
	try
	{
		std::string username = user;
		unsigned char* client_id = new unsigned char[17];
		std::vector<std::string> encryped_and_pk;
		std::string private_key;
		std::string encrypted_aes_key;
		try // experiment to register in the system
		{
			bool ex_file = utils::exist_meInfo();  //Checking whether the Me.info file exist
			if (ex_file) {   //client exist
				//A function that does re-registration
				std::cout << "Logging back There is a me.info file \n" << std::endl;
				auto tcp_client = client_request::InitClient();
				throw ExceptionUserRegistered(std::exception());

			}
			bool Succeeded = FALSE;
			for (size_t i = 0; i < NUMBER_TRIES_TO_SEND; i++)
			{
				if (Succeeded == FALSE) {
					try
					{
						//The customer is registered in the system
						auto tcp_client = client_request::InitClient();
						client_id = client_request::RegistrationReq(tcp_client, username);
						utils::writeToFile("me.info.txt", tcp_client.getName());  // write the name in me.info
						std::cout << "registration succeeded \n" << std::endl;
						utils::uuid_to_hex(client_id, SIZE_UUID);   // write the uuid in hex in me.info
						Succeeded = TRUE;
					}
					catch (const ExceptionRegistrationFailed & e)
					{
						Succeeded = FALSE;
					}
				}
				else
				{
					break;
				}
			}
			//The customer registers in the system and sends a public key
			auto tcp_client = client_request::InitClient();
			encryped_and_pk = client_request::SendPK(tcp_client, client_id, username);
			private_key = encryped_and_pk.front();
			encrypted_aes_key = encryped_and_pk.back();
		}
		catch (const ExceptionUserRegistered& e)
		{
			//The user exists, so we will re-register
			try
			{
				auto tcp_client = client_request::InitClient();
				client_request::RequestToReconnect(tcp_client, encrypted_aes_key, client_id);
				//read privatekey from me.info 
				std::string private_key_base64key = utils::readMeFile(1); //to read line 2
				private_key = Base64Wrapper::decode(private_key_base64key);
			}
			catch (const ExceptionReconnecting & e)
			{
				std::cout << "trying to register again";
				auto tcp_client = client_request::InitClient();
				client_id = client_request::RegistrationReq(tcp_client, username);
				utils::writeToFile("me.info.txt", tcp_client.getName());
				std::cout << "registration succeeded \n" << std::endl;
				utils::uuid_to_hex(client_id, SIZE_UUID);   // write the uuid in hex in me.info
				encryped_and_pk = client_request::SendPK(tcp_client, client_id, username);
				private_key = encryped_and_pk.front();
				encrypted_aes_key = encryped_and_pk.back();
			}
		}
		auto tcp_client = client_request::InitClient();
		std::string decrypted_aes_key = client_request::DecryptAESKey(encrypted_aes_key, private_key);
		std::string fileContent = utils::readFileClient(tcp_client.getfile_path());
		std::string ciphertext = client_request::EncryptFileAES(decrypted_aes_key, fileContent);
		std::cout << "[INFO] Sending encrypted file" << std::endl;

		bool valid_crc = false;

		for (size_t i = 0; i < NUMBER_TRIES_TO_SEND; i++) {
			std::cout << i+1<< "\n" << std::endl;
			valid_crc = client_request::SendEncryptedFile(tcp_client, client_id, ciphertext);
			if (valid_crc)
			{
				std::cout << "File recieved succesfully! \n" << std::endl;
				client_request::SendValidCrc(tcp_client, client_id);
				break;
			}

			else
				client_request::SendInvalidCRC(tcp_client, client_id);
		}

		if (!valid_crc) {
			client_request::SendTerminatingSessionInvalidCRC(tcp_client, client_id);
		}
		std::cout << " BY";
		delete client_id;
	}
	catch (const std::exception& e )
	{
		std::cout << e.what();
	}
}


