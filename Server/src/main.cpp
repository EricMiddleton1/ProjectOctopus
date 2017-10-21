#include <iostream>

#include <mrpt/maps/CColouredPointsMap.h>

#include "server.hpp"

void server_callback(Json::Value root);

int main() {
    Server server(1, server_callback, 8080);

    while (1) {
        server.processReads();
    }

    return 0;
}

void server_callback(Json::Value root) {
    std::cout << "Got a callback" << std::endl;
    int val;
    try {
        val = root.get("thing", -1).asInt();
    }
    catch (std::exception& e) {
        std::cout << "Exception when reading JSON: " << e.what() << std::endl;
    }
    std::cout << "thing = " << val << std::endl;
}