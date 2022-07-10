#ifndef DIGITAL_SIGNATURE_H
#define DIGITAL_SIGNATURE_H

#include <cryptopp/cryptlib.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/pssr.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/asn.h>
#include <cryptopp/pubkey.h>
#include <cryptopp/oids.h>
#include <cryptopp/md5.h>

#include "picosha2.h"

#include <fstream>
#include <sstream>

#ifdef _WIN64

const std::string CRYPTO_DIR_PATH = std::string(getenv("APPDATA")) + "\\.crypto\\";
const std::string PRIVATE_KEY_PATH = CRYPTO_DIR_PATH + "private_key.der";
const std::string PUBLIC_KEY_PATH = CRYPTO_DIR_PATH + "public_key.der";

#endif

#ifdef __linux__

#define CRYPTO_DIR_PATH "TODO"

#endif

class DigitalSignature
{

private:
public:
	static std::string get_public_key()
	{

		std::ifstream f(PUBLIC_KEY_PATH);

		std::stringstream ss;

		ss << f.rdbuf();

		return ss.str();
	}

	static std::string get_private_key()
	{

		std::ifstream f(PRIVATE_KEY_PATH);

		std::stringstream ss;

		ss << f.rdbuf();

		return ss.str();
	}

	static std::string calculate_address(const std::string &public_key)
	{

		std::string address;

		std::string hash_s;
		std::string hash_d;

		CryptoPP::SHA1 sha1;

		picosha2::hash256_hex_string(public_key, hash_s);

		CryptoPP::StringSource(hash_s, true,
							   new CryptoPP::HashFilter(sha1,
														new CryptoPP::HexEncoder(
															new CryptoPP::StringSink(hash_d))));

		picosha2::hash256_hex_string(hash_d, hash_s);
		picosha2::hash256_hex_string(hash_s, hash_s);

		hash_d.append(std::string(hash_s.end() - 4, hash_s.end()));

		std::string decoded;

		CryptoPP::StringSource ss(hash_d, true, new CryptoPP::StringSink(decoded));
		const CryptoPP::byte *data = reinterpret_cast<const CryptoPP::byte *>(decoded.data());

		CryptoPP::StringSource address_ss(data, sizeof(hash_d), true,
										  new CryptoPP::Base64Encoder(
											  new CryptoPP::StringSink(address)));

		return address;
	}

	static std::pair<std::string, std::string> generate_keys_pair()
	{

		CryptoPP::AutoSeededRandomPool rng;

		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey private_key;
		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey public_key;

		private_key.Initialize(rng, CryptoPP::ASN1::secp256k1());

		private_key.MakePublicKey(public_key);

		std::string private_key_str;
		std::string public_key_str;

		std::wstring w_str = std::wstring(CRYPTO_DIR_PATH.begin(), CRYPTO_DIR_PATH.end());

		if (_wmkdir(w_str.c_str()) == 0);
			// Todo: add exception
			// throw std::exception("Directory error!");

		CryptoPP::Base64Encoder private_key_sink(new CryptoPP::StringSink(private_key_str));

		private_key.DEREncode(private_key_sink);
		private_key_sink.MessageEnd();

		CryptoPP::Base64Encoder public_key_sink(new CryptoPP::StringSink(public_key_str));
		public_key.DEREncode(public_key_sink);
		public_key_sink.MessageEnd();

		return std::make_pair(public_key_str, private_key_str);
	}

	static std::string sign_hash(const std::string &to_sign, const std::string &private_key_str)
	{

		CryptoPP::AutoSeededRandomPool rng;
		std::string signature;

		CryptoPP::ByteQueue bytes;
		CryptoPP::StringSource str(private_key_str, true, new CryptoPP::Base64Decoder);
		str.TransferTo(bytes);
		bytes.MessageEnd();
		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey private_key;
		private_key.Load(bytes);

		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer(private_key);

		CryptoPP::StringSource ss(to_sign, true,
								  new CryptoPP::SignerFilter(rng, signer,
															 new CryptoPP::HexEncoder(
																 new CryptoPP::StringSink(signature))));

		return signature;
	}

	static bool verify_signature(const std::string &signed_string, const std::string &signature, const std::string &public_key_string)
	{

		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey public_key;

		public_key.Load(CryptoPP::StringSource(public_key_string, true,
											   new CryptoPP::Base64Decoder)
							.Ref());

		bool result = false;

		CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier verifier(public_key);

		std::string decoded_signature;
		CryptoPP::StringSource ss(signature, true,
								  new CryptoPP::HexDecoder(
									  new CryptoPP::StringSink(decoded_signature)));

		CryptoPP::StringSource ss2(decoded_signature + signed_string, true,
								   new CryptoPP::SignatureVerificationFilter(verifier,
																			 new CryptoPP::ArraySink((CryptoPP::byte *)&result,
																									 sizeof(result))));

		return result;
	}
};

#endif