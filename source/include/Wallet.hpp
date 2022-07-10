#ifndef WALLET_H
#define WALLET_H

#include <string>
#include "json.hpp"
#include "picosha2.h"
#include <fstream>
#include <sstream>
#include "DigitalSignature.hpp"

inline bool exists(const std::string &name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

struct Wallet
{
private:
public:
	std::string address;
	std::string public_key;

	Wallet()
	{

		if (exists(PRIVATE_KEY_PATH) && exists(PUBLIC_KEY_PATH))
		{

			std::fstream pvr_k_f(PRIVATE_KEY_PATH);
			std::stringstream pvr_k_ss;

			std::fstream pbl_k_f(PUBLIC_KEY_PATH);
			std::stringstream pbl_k_ss;

			pvr_k_ss << pvr_k_f.rdbuf();
			pbl_k_ss << pbl_k_f.rdbuf();

			this->address = picosha2::hash256_hex_string(pbl_k_ss.str());
			this->public_key = pbl_k_ss.str();
		}
		else
		{

			std::wstring w_str = std::wstring(CRYPTO_DIR_PATH.begin(), CRYPTO_DIR_PATH.end());

			int result = _wmkdir(w_str.c_str());

			std::string generated_key = DigitalSignature::generate_keys_pair();

			this->address = picosha2::hash256_hex_string(generated_key);
			this->public_key = generated_key;
		}
	}

	operator std::string() const
	{

		nlohmann::json j = {

			{"address", this->address},
			{"public_key", this->public_key},

		};

		return j.dump(4);
	}
};

void from_json(const nlohmann::json &j, Wallet &w)
{

	j.at("address").get_to(w.address);
	j.at("public_key").get_to(w.public_key);
}

void to_json(nlohmann::json &j, const Wallet &w)
{
	j = {

		{"address", w.address},
		{"public_key", w.public_key}

	};
}

#endif