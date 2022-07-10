#ifndef BLOCK_H
#define BLOCK_H

#define NONCE_MAX_RANGE 1000
#define POW_LENGTH 3

#include "Transaction.h"
#include <vector>
#include "json.hpp"

class Block
{
private:

	std::vector<Transaction> transactions;

	std::string previous;

	std::string hash;

	int nonce;

	std::string calculate_hash() const;


public:

	Block();
	Block(std::vector<Transaction>);

	std::vector<Transaction> get_transaction() const { return this->transactions; }
	std::string get_hash() const { return this->calculate_hash(); }
	std::string get_previous() const { return this->previous; }
	int get_nonce() const { return this->nonce; }

	void set_transactions(std::vector<Transaction> transactions) { this->transactions = transactions; }
	void set_hash(std::string hash) { this->hash = hash; }
	void set_previous(std::string previous) { this->previous = previous; }
	void set_nonce(int nonce) { this->nonce = nonce; }

	int add_transaction(Transaction);
	int chain_previous(std::string);

	void pow();

	bool is_valid();

	nlohmann::json to_json();

};

void from_json(const nlohmann::json& j, Block& b);

void to_json(nlohmann::json& j, const Block& b);

#endif