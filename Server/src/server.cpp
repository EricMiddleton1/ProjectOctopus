#include <mrpt/utils/CClientTCPSocket.h>
#include <jsoncpp/json/reader.h>

#include <vector>
#include <string>

#include "server.hpp"
using namespace mrpt::utils;

Server::Server(const SCb callback, unsigned short listenPort, std::string ipAddr) : serverSock(listenPort, ipAddr) {
    cb_func = callback;

}

void Server::start() {
    while (1) {
        CClientTCPSocket* req_client = serverSock.accept();
        
        // Read length byte
        uint32_t data_len = 15;
        size_t read_len = req_client->ReadBuffer(&data_len, sizeof(data_len));
        if (!read_len) {
            std::cout << "Error reading length header from client" << std::endl;
            delete req_client;
            continue;
        }
        
        // Read the bulk of the data
        std::vector<char> raw_data(data_len);
        req_client->ReadBuffer(&raw_data[0], data_len);
        std::cout << raw_data[0] << std::endl;
        // Convert to JSON
        Json::Reader reader;
        Json::Value json_root;
        bool parsing_ok = reader.parse(&raw_data[0], &raw_data[raw_data.size()], json_root);
        if (!parsing_ok) {
            std::cout << "Failed to parse JSON: " << std::endl << std::string(raw_data.begin(), raw_data.end()) << std::endl;
            delete req_client;
            continue;
        }

        cb_func(json_root);

        delete req_client;
    }
}