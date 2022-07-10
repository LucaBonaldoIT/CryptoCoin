#include "Block.h"

Block::Block()
{

	previous = "Block Chain Start";

	nonce = rand() % NONCE_MAX_RANGE;

	transactions = {};

	hash = this->calculate_hash();
}

Block::Block(std::vector<Transaction> transactions)
{

	this->transactions = transactions;

	this->nonce = rand() % NONCE_MAX_RANGE;

	this->hash = this->calculate_hash();
}

int Block::add_transaction(Transaction t)
{
	try
	{

		transactions.push_back(t);

		this->hash = this->calculate_hash();
	}
	catch (...)
	{

		// TODO - Aggiungere sistema di catch delle eccezioni

		return -1;
	}

	return 0;
}

int Block::chain_previous(std::string previous)
{
	try
	{

		this->previous = previous;
		this->hash = this->calculate_hash();

		return 0;
	}
	catch (...)
	{

		// TODO - Gi� lo sai

		return -1;
	}
}

std::string Block::calculate_hash() const
{

	std::string hash_hex;

	std::string transactions_hashes;

	for (Transaction t : this->transactions)
	{

		transactions_hashes += t.get_hash();
	}

	picosha2::hash256_hex_string(transactions_hashes + previous + std::to_string(this->nonce), hash_hex);

	return hash_hex;
}

void Block::pow()
{

	do
	{

		if (!is_valid())
		{
			this->nonce += 1;
			hash = this->calculate_hash();
		}

	} while (!is_valid());
}

bool Block::is_valid()
{
	for (size_t i = 0; i < POW_LENGTH; i++)
		if (hash.at(i) != '0')
			return false;

	return true;
}

nlohmann::json Block::to_json()
{
	nlohmann::json j = {

		{"hash", hash},
		{"previous", previous},
		{"nonse", nonce},
		{"transactions", transactions}

	};

	return j;
}

void from_json(const nlohmann::json &j, Block &b)
{

	b.set_hash(j.at("hash"));
	b.set_previous(j.at("previous"));
	b.set_nonce(j.at("nonce"));
	b.set_transactions(j.at("transactions"));
}

void to_json(nlohmann::json &j, const Block &b)
{

	j = {

		{"hash", b.get_hash()},
		{"previous", b.get_previous()},
		{"nonce", b.get_nonce()},
		{"transactions", b.get_transaction()}

	};
}