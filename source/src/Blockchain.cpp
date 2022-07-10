#include "Blockchain.h"

int Blockchain::add_block(Block b)
{

	try
	{

		b.chain_previous(blocks.at(blocks.size() - 1).get_hash());

		blocks.push_back(b);

		return 0;
	}
	catch (...)
	{

		// TODO sistema catch exceptions
		return -1;
	}
}

Blockchain::Blockchain(std::pair<std::string, std::string> keys, bool generate_new)
{

	int result = _wmkdir(std::wstring(CRYPTO_DIR_PATH.begin(), CRYPTO_DIR_PATH.end()).c_str());

	if (generate_new)
	{
		this->genesis(keys);
		this->save();
	}
	else
	{
		try
		{
			this->load();
		}
		catch (CryptoPP::FileStore::OpenErr const &)
		{
			std::cerr << "Error: " << strerror(errno) << '\n';
		}
		catch (...)
		{
			std::cout << "Exception reading the blockchain... Generating a new one.";
			exit(-1);
		}
	}
}
int Blockchain::genesis(std::pair<std::string, std::string> keys)
{

	Transaction first_funds = Transaction("INITIAL_FUNDS",
										  DigitalSignature::calculate_address(keys.first), keys.first,
										  INITIAL_FUNDS);

	first_funds.sign(keys.second);

	Block b = Block();

	b.add_transaction(first_funds);

	b.pow();

	this->blocks.push_back(b);

	return 0;
}

int Blockchain::save()
{
	std::ofstream ofs(BLOCKCHAIN_PATH);
	ofs << this->to_json();
	ofs.close();
	return 0;
}

int Blockchain::load()
{
	std::ifstream ifs(BLOCKCHAIN_PATH);
	nlohmann::json j;

	ifs >> j;
	ifs.close();

	from_json(j, *this);

	return 0;
}

bool Blockchain::mine()
{

	size_t pending_transactions_number = pending_transactions.size();
	size_t last_pending_transaction_index = 0;

	if (pending_transactions_number <= BLOCK_SIZE)
	{
		// throw "Not enough pending transactions! (Must be: >" + std::to_string(BLOCK_SIZE) +")";
		return false;
	}

	std::vector<Transaction> transactions_to_mine = std::vector_utility::slice(pending_transactions, 0, BLOCK_SIZE);

	Block block_to_add = Block(transactions_to_mine);

	block_to_add.chain_previous(blocks.at(blocks.size() - 1).get_hash());

	block_to_add.pow();

	if (!block_to_add.is_valid())
	{
		throw "Not valid hash! Should start with \"00000\"";
		return false;
	}

	this->pending_transactions = std::vector_utility::slice(pending_transactions, BLOCK_SIZE + 1, pending_transactions_number - 1);
	blocks.push_back(block_to_add);

	return true;
}

bool Blockchain::is_valid()
{
	for (size_t i = 1; i < blocks.size(); i++)
	{

		for (Transaction t : blocks.at(i).get_transaction())
		{

			if (!t.is_valid())
				return false;
		}

		if (blocks.at(i).get_previous() != blocks.at(i - 1).get_hash())
			return false;

		if (!blocks.at(i).is_valid())
			return false;
	}

	return true;
}

double Blockchain::get_balance(const std::string &address)
{
	{

		double balance = 0;

		for (Block b : blocks)
			for (Transaction t : b.get_transaction())
			{
				if (t.get_sender_address() == address)
					balance -= t.get_amount();
				else if (t.get_receiver_address() == address)
					balance += t.get_amount();
			}

		for (Transaction t : this->pending_transactions)
		{
			if (t.get_sender_address() == address)
				balance -= t.get_amount();
			else if (t.get_receiver_address() == address)
				balance += t.get_amount();
		}

		return balance;
	}
}

int Blockchain::add_pending_transaction(Transaction t)
{
	if (t.get_amount() <= 0)
		return INVALID_AMOUNT;
	if (this->get_balance(t.get_sender_address()) - t.get_amount() < 0)
		return INSUFFICIENT_FUNDS;
	if (!t.is_valid())
		return -3;
	else
	{
		pending_transactions.push_back(t);
		return SUCCESS;
	}
}

nlohmann::json Blockchain::to_json()
{

	nlohmann::json j = {

		{"blocks", this->blocks},
		{"pending_transactions", this->pending_transactions}

	};

	return j;
}

void from_json(const nlohmann::json &j, Blockchain &b)
{
	b.set_blocks(j.at("blocks"));
	b.set_pending_transactions(j.at("pending_transactions"));
}

void to_json(nlohmann::json &j, const Blockchain &b)
{

	j = {
		{"blocks", b.get_blocks()},
		{"pending_transactions", b.get_pending_transactions()}

	};
}