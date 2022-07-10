#ifndef SESSION_H
#define SESSION_H

#include <string>

class Session
{

private:
	std::string ip;

	std::string address;

	std::string public_key;
	std::string private_key;

public:
	std::string get_ip() { return this->ip; }
	std::string get_address() { return this->address; }
	std::string get_public_key() { return this->public_key; }
	std::string get_private_key() { return this->private_key; }

	void set_ip(std::string ip) { this->ip = ip; }
	void set_address(std::string adress) { this->address = adress; }
	void set_public_key(std::string public_key) { this->public_key = public_key; }
	void set_private_key(std::string private_key) { this->private_key = private_key; }

	Session(std::string ip, std::string address, std::string public_key, std::string private_key) : ip(ip), address(address), public_key(public_key), private_key(private_key) {}

	Session() : ip("NULL"), address("NULL"), public_key("NULL"), private_key("NULL") {}
	Session(std::string ip) : ip(ip), address("NULL"), public_key("NULL"), private_key("NULL") {}
};

#endif