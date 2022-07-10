#include "../include/Transaction.h"

std::string Transaction::calculate_hash() const
{

	std::string hash_hex;

	std::stringstream ss;
	ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %X");

	picosha2::hash256_hex_string(sender_address + receiver_address + std::to_string(amount) + ss.str(), hash_hex);

	return hash_hex;
}

Transaction::Transaction()
{

	this->sender_address = "NONE";
	this->receiver_address = "NONE";

	this->public_key = "NONE";

	this->amount = 0;

	this->timestamp = time(nullptr);

	this->hash = this->calculate_hash();
	this->signature = "NOT_SIGNED";
}

Transaction::Transaction(std::string sender_address, std::string receiver_address, std::string public_key, double amount)
{

	this->sender_address = sender_address;
	this->receiver_address = receiver_address;

	this->public_key = public_key;

	this->amount = amount;

	this->timestamp = time(nullptr);

	this->hash = this->calculate_hash();

	this->signature = "NOT_SIGNED";
}

int Transaction::sign(const std::string &private_key)
{

	this->signature = DigitalSignature::sign_hash(this->calculate_hash(), private_key);

	return 0;
}

Transaction::operator std::string()
{

	return this->to_json().dump(4);
}

bool Transaction::is_valid()
{

	bool result = false;

	try
	{
		result = DigitalSignature::verify_signature(this->calculate_hash(), this->signature, this->public_key);
	}
	catch (...)
	{

		std::cout << "Error!";

		return false;
	}

	return result;
}

nlohmann::json Transaction::to_json()
{
	nlohmann::json j = {

		{"sender_address", this->sender_address},
		{"receiver_address", this->receiver_address},
		{"public_key", this->public_key},
		{"amount", this->amount},
		{"timestamp", this->timestamp},
		{"hash", this->calculate_hash()},
		{"signature", this->signature}

	};

	return j;
}

void to_json(nlohmann::json &j, const Transaction &t)
{

	j = {

		{"sender_address", t.get_sender_address()},
		{"receiver_address", t.get_receiver_address()},
		{"public_key", t.get_public_key()},
		{"amount", t.get_amount()},
		{"timestamp", t.get_timestamp()},
		{"hash", t.get_hash()},
		{"signature", t.get_signature()},
		{"public_key", t.get_public_key()}

	};
}

void from_json(const nlohmann::json &j, Transaction &t)
{

	t.set_sender_address(j.at("sender_address"));
	t.set_receiver_address(j.at("receiver_address"));
	t.set_amount(j.at("amount"));
	t.set_timestamp(j.at("timestamp"));
	t.set_hash(j.at("hash"));
	t.set_signature(j.at("signature"));
	t.set_public_key(j.at("public_key"));
}
