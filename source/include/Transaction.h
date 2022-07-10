#ifndef TRANSACTION_H
#define TRANSACTION_H

#pragma warning(disable : 4996)

#include <chrono>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <map>

#include <boost/any.hpp>
#include <boost/functional/hash_fwd.hpp>

#include "json.hpp"
#include "picosha2.h"

#include <string>
#include <vector>

#include "DigitalSignature.hpp"

class Transaction
{

private:
	std::string sender_address;
	std::string receiver_address;

	std::string public_key;

	float amount;

	time_t timestamp;

	std::string hash;
	std::string signature;

public:
	Transaction(std::string, std::string, std::string, double);
	Transaction();

	void set_sender_address(std::string sender_address) { this->sender_address = sender_address; }
	void set_receiver_address(std::string receiver_address) { this->receiver_address = receiver_address; }
	void set_public_key(std::string public_key) { this->public_key = public_key; }
	void set_amount(float amount) { this->amount = amount; }
	void set_timestamp(time_t timestamp) { this->timestamp = timestamp; }
	void set_hash(std::string hash) { this->hash = hash; }
	void set_signature(std::string signature) { this->signature = signature; }

	std::string get_sender_address() const { return this->sender_address; }
	std::string get_receiver_address() const { return this->receiver_address; }
	std::string get_public_key() const { return this->public_key; }
	float get_amount() const { return this->amount; }
	time_t get_timestamp() const { return this->timestamp; }
	std::string get_hash() const { return this->calculate_hash(); }
	std::string get_signature() const { return this->signature; }

	std::string calculate_hash() const;
	nlohmann::json to_json();

	bool is_valid();
	operator std::string();
	int sign(const std::string &);
};

namespace std
{

	namespace vector_utility
	{

		template <typename T>
		std::vector<T> slice(std::vector<T> const &v, int m, int n)
		{
			{
				auto first = v.cbegin() + m;
				auto last = v.cbegin() + n + 1;

				std::vector<T> vec(first, last);
				return vec;
			}
		}

	}
}

void to_json(nlohmann::json &j, const Transaction &t);
void from_json(const nlohmann::json &j, Transaction &t);

#endif