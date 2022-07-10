#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "Block.h"
#include "DigitalSignature.hpp"

#define BLOCK_SIZE 2

#define INSUFFICIENT_FUNDS -1
#define INVALID_AMOUNT -2
#define SUCCESS 0

#define INITIAL_FUNDS 100

#ifdef _WIN64

const std::string BLOCKCHAIN_PATH = CRYPTO_DIR_PATH + "blockchain.dat";

#endif

#ifdef __linux__

#define CRYPTO_DIR_PATH "TODO"

#endif

class Blockchain
{
private:
	std::vector<Block> blocks;
	std::vector<Transaction> pending_transactions;

	int add_block(Block b);

public:
	/**
	 *
	 * Create or load the blockchain.
	 *
	 * Create or load the blockchain.
	 *
	 * @param generate_new specify if the blockchain should be created or loaded
	 *
	 */

	Blockchain(std::pair<std::string, std::string> keys, bool generate_new = false);

	std::vector<Block> get_blocks() const { return this->blocks; }
	std::vector<Transaction> get_pending_transactions() const { return this->pending_transactions; }

	void set_blocks(std::vector<Block> blocks) { this->blocks = blocks; }
	void set_pending_transactions(std::vector<Transaction> pending_transactions) { this->pending_transactions = pending_transactions; }
	int genesis(std::pair<std::string, std::string> keys);

	int save();
	int load();

	bool mine();

	bool is_valid();

	double get_balance(const std::string &);

	operator std::string() { return this->to_json().dump(4); }

	int add_pending_transaction(Transaction);

	nlohmann::json to_json();
};

void from_json(const nlohmann::json &j, Blockchain &b);

void to_json(nlohmann::json &j, const Blockchain &b);

#endif