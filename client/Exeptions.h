#pragma once
#include <string>
#include <iostream>




class ExceptionUserRegistered : public std::exception {
private:
	std::string msg;
public:
	ExceptionUserRegistered(const std::exception e);
	virtual const char* what() const throw();
};

class ExceptionReconnecting : public std::exception {
private:
	std::string msg;
public:
	ExceptionReconnecting(const std::exception e);
	virtual const char* what() const throw();
};


class ExceptionDecryptAESKey : public std::exception {
private:
	std::string msg;
public:
	ExceptionDecryptAESKey(const std::exception e);
	virtual const char* what() const throw();
};


class ExceptionRegistrationFailed : public std::exception {
private:
	std::string msg;
public:
	ExceptionRegistrationFailed(const std::exception e);
	virtual const char* what() const throw();
};



