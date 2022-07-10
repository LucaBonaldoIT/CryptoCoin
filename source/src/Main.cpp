#define CROW_MAIN
#include "../include/crow_all.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include "../include/Transaction.h"
#include "../include/Block.h"
#include "../include/Blockchain.h"
#include "../include/Session.h"

#include <vector>
//#include "Wallet.hpp"
#include "DigitalSignature.hpp"
#include "json.hpp"

/**
 *
 * Entry point of the program.
 *
 * The program starts parsing the commands given
 * specified by the arguments argc and argv.
 *
 * @param argc Lenght of argv[].
 *
 * @param argv[] Commands given to the program.
 *
 */

/// Global variable of the app

namespace server
{

    // Key pair
    std::pair<std::string, std::string> keys;

    // Public key
    std::string public_key;

    // Private key
    std::string private_key;

    // Address
    std::string address;

    // Blockchain loading and init
    Blockchain *blockchain;

    // Server session
    Session *session;

    // Sessions map

    std::map<std::string, Session> sessions;

}

// Utilities function (not worth a class)

Session get_session(std::string); // Find the corresponding session or creates a new one

std::string find_and_replace( // Replace occurence in a string with another
    std::string &s,
    const std::string &to_replace,
    const std::string &replace_with);

int main(int argc, char *argv[])
{

    // Initialize Crow app

    crow::SimpleApp app;

    if (argc == 1)
    {
        server::blockchain = new Blockchain(std::make_pair("TMP", "TMP"), false);
    }
    else if (argc != 2)
    {
        return -1;
    }
    else if (std::string(argv[1]) == "-load")
    {
        server::blockchain = new Blockchain(std::make_pair("TMP", "TMP"), false);
    }
    else if (std::string(argv[1]) == "-new")
    {

        // Create server credentials

        server::keys = DigitalSignature::generate_keys_pair();

        server::public_key = server::keys.first;
        server::private_key = server::keys.second;
        server::address = DigitalSignature::calculate_address(server::public_key);

        // Inialize blockchain

        server::blockchain = new Blockchain(server::keys, true);

        // Initialize server session

        server::session = new Session("127.0.0.1", server::address, server::public_key, server::private_key);
        server::sessions["127.0.0.1"] = *(server::session);
    }
    else
    {
        return -1;
    }

    // List of app routes

    CROW_ROUTE(app, "/")
    ([]
     {

        crow::mustache::template_t page = crow::mustache::load("index.htm");
        
        return page.render(); });

    CROW_ROUTE(app, "/account").methods("GET"_method, "POST"_method)([](const crow::request &req, crow::response &res)
                                                                     {
                                                                         crow::mustache::template_t page = crow::mustache::load("account.htm");

                                                                         std::string req_ip = req.remote_ip_address;

                                                                         Session current_session = get_session(req_ip);

                                                                         if (current_session.get_address() != "NULL")
                                                                         {

                                                                             crow::mustache::template_t page = crow::mustache::load("account_signed.htm");

                                                                             crow::mustache::context ctx;

                                                                             ctx["address"] = current_session.get_address();
                                                                             ctx["balance"] = server::blockchain->get_balance(current_session.get_address());

                                                                             res.body = page.render(ctx);
                                                                         }

                                                                         else
                                                                         {
                                                                             res.body = page.render();
                                                                         }

                                                                         res.end();
                                                                     });

    CROW_ROUTE(app, "/account/generate").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                               {
                                                                   std::string req_ip = req.remote_ip_address;

                                                                   auto keys = DigitalSignature::generate_keys_pair();

                                                                   server::sessions[req_ip].set_address(DigitalSignature::calculate_address(keys.first));

                                                                   server::sessions[req_ip].set_public_key(keys.first);
                                                                   server::sessions[req_ip].set_private_key(keys.second);

                                                                   std::ofstream file_gen("file_gen.txt");
                                                                   file_gen << keys.first << "\n"
                                                                            << keys.second;
                                                                   file_gen.close();

                                                                   res.redirect("/account");
                                                                   res.end();

                                                                   return;
                                                               });

    CROW_ROUTE(app, "/account/signin").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                             {
                                                                 std::string req_ip = req.remote_ip_address;

                                                                 if (req.url_params.get("public_key") == nullptr || req.url_params.get("private_key") == nullptr)
                                                                     res.redirect("/account");

                                                                 else
                                                                 {

                                                                     std::string public_key = req.url_params.get("public_key") + std::string("\n");
                                                                     std::string private_key = req.url_params.get("private_key") + std::string("\n");

                                                                     find_and_replace(public_key, "\r\n", "\n");
                                                                     find_and_replace(private_key, "\r\n", "\n");

                                                                     server::sessions[req_ip].set_address(DigitalSignature::calculate_address(public_key));

                                                                     server::sessions[req_ip].set_private_key(private_key);
                                                                     server::sessions[req_ip].set_public_key(public_key);

                                                                     res.redirect("/account");
                                                                 }

                                                                 res.end();

                                                                 return;
                                                             });

    CROW_ROUTE(app, "/account/public").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                             {
                                                                 std::string req_ip = req.remote_ip_address;

                                                                 Session current_session = get_session(req_ip);

                                                                 res.body = current_session.get_public_key();
                                                                 res.end();

                                                                 return;
                                                             });

    CROW_ROUTE(app, "/account/private").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                              {
                                                                  std::string req_ip = req.remote_ip_address;

                                                                  Session current_session = get_session(req_ip);

                                                                  res.body = current_session.get_private_key();
                                                                  res.end();

                                                                  return;
                                                              });

    CROW_ROUTE(app, "/account/logout").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                             {
                                                                 std::string req_ip = req.remote_ip_address;

                                                                 server::sessions[req_ip] = Session(req_ip);

                                                                 res.redirect("/account");
                                                                 res.end();

                                                                 return;
                                                             });

    CROW_ROUTE(app, "/transaction").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                          {
                                                              crow::mustache::template_t page = crow::mustache::load("transaction.htm");
                                                              crow::mustache::context ctx;

                                                              std::string req_ip = req.remote_ip_address;

                                                              Session current_session = get_session(req_ip);

                                                              if (current_session.get_address() == "NULL")
                                                              {
                                                                  res.redirect("/account");
                                                                  res.end();
                                                                  return;
                                                              }

                                                              ctx["balance"] = server::blockchain->get_balance(current_session.get_address());

                                                              res.body = page.render(ctx);
                                                              res.end();

                                                              return;
                                                          });

    CROW_ROUTE(app, "/transaction/process").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                                  {
                                                                      crow::mustache::template_t page = crow::mustache::load("transaction.htm");
                                                                      crow::mustache::context ctx;

                                                                      std::string req_ip = req.remote_ip_address;

                                                                      Session current_session = get_session(req_ip);

                                                                      if (req.url_params.get("address") == nullptr ||
                                                                          req.url_params.get("amount") == nullptr ||
                                                                          (req.url_params.get("address") + std::string("\n")) == current_session.get_address() ||
                                                                          std::stod(req.url_params.get("amount")) <= 0 ||
                                                                          std::stod(req.url_params.get("amount")) > server::blockchain->get_balance(current_session.get_address()))
                                                                      {

                                                                          res.redirect("/transaction");
                                                                          res.end();
                                                                          return;
                                                                      }

                                                                      std::string sender_address = current_session.get_address();

                                                                      std::string receiver_address = req.url_params.get("address") + std::string("\n");

                                                                      std::string public_key = current_session.get_public_key();

                                                                      std::string private_key = current_session.get_private_key();

                                                                      double amount = std::stod(req.url_params.get("amount"));

                                                                      Transaction to_add = Transaction(sender_address, receiver_address, public_key, amount);

                                                                      to_add.sign(private_key);

                                                                      server::blockchain->add_pending_transaction(to_add);

                                                                      res.redirect("/account");
                                                                      res.end();

                                                                      return;
                                                                  });

    CROW_ROUTE(app, "/info").methods("GET"_method, "POST"_method)([](const crow::request &req)
                                                                  {
                                                                      crow::mustache::template_t page = crow::mustache::load("info.htm");

                                                                      std::string req_ip = req.remote_ip_address;

                                                                      Session current_session = get_session(req_ip);

                                                                      return page.render();
                                                                  });

    CROW_ROUTE(app, "/explore")
    ([]()
     {

        crow::mustache::template_t page = crow::mustache::load("explore.htm");

        return page.render(); });

    CROW_ROUTE(app, "/mine")
    ([]()
     {

        crow::mustache::template_t page = crow::mustache::load("mine.htm");
        crow::mustache::context ctx;
        
        ctx["transactions_number"] = server::blockchain->get_pending_transactions().size();
        ctx["blocks_number"] = int(server::blockchain->get_pending_transactions().size() / BLOCK_SIZE);

        return page.render(ctx); });

    CROW_ROUTE(app, "/mine/process").methods("GET"_method)([](const crow::request &req, crow::response &res)
                                                           {
                                                               std::string req_ip = req.remote_ip_address;

                                                               Session current_session = get_session(req_ip);

                                                               if (server::blockchain->mine())
                                                               {
                                                                   server::blockchain->save();
                                                                   res.redirect("/mine/process");
                                                               }
                                                               else
                                                               {
                                                                   res.redirect("/account");
                                                               }

                                                               res.end();

                                                               return;
                                                           });

    system("START \"\" \"http://localhost:18080\"");

    // Once everything is set, we can run the site
    app.port(18080).multithreaded().run();

    return 0;
}

Session get_session(std::string ip)
{

    if (server::sessions.count(ip) > 0)
    {
        return server::sessions[ip];
    }
    else
    {
        server::sessions[ip] = Session(ip);
        return server::sessions[ip];
    }
}

std::string find_and_replace(
    std::string &s,
    const std::string &to_replace,
    const std::string &replace_with)
{
    std::size_t pos = s.find(to_replace);
    if (pos == std::string::npos)
        return s;
    return s.replace(pos, to_replace.length(), replace_with);
}