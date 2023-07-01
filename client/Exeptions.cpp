#include "Exeptions.h"




ExceptionUserRegistered::ExceptionUserRegistered(const std::exception e)
{
	std::string formatted_msg = std::string();
	formatted_msg = "User is already registered! (Code 2101)";
	formatted_msg += e.what();
	msg = formatted_msg.c_str();
}

const char* ExceptionUserRegistered::what() const throw() {
	std::cout << msg;
	return msg.c_str();
}

ExceptionReconnecting::ExceptionReconnecting(const std::exception e)
{
	std::string formatted_msg = std::string();
	formatted_msg = "The user does not exist (Code 2106)";
	formatted_msg += e.what();
	msg = formatted_msg.c_str();
}

const char* ExceptionReconnecting::what() const throw() {
	std::cout << msg;
	return msg.c_str();
}

ExceptionDecryptAESKey::ExceptionDecryptAESKey(const std::exception e) {
	std::string formatted_msg = std::string();
	formatted_msg = "An error occured during the decryption of the AES key. \n Might be a data corruption of the server. \n Please try again.";
	formatted_msg += e.what();

	msg = formatted_msg.c_str();
}

const char* ExceptionDecryptAESKey::what() const throw() {
	return msg.c_str();
}

ExceptionRegistrationFailed::ExceptionRegistrationFailed(const std::exception e) {
	std::string formatted_msg = std::string();
	formatted_msg = "The name already exists in the system, \n Please try again.";
	formatted_msg += e.what();

	msg = formatted_msg.c_str();
}

const char* ExceptionRegistrationFailed::what() const throw() {
	return msg.c_str();
}

