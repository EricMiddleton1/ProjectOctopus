#include <iostream>
#include <functional>

#include "server.hpp"
#include "SLAMmer.hpp"

// void server_callback(Json::Value root);


int main() {
    SLAMmer slammer;

    auto callback = [&slammer] (const Json::Value& root) {
        slammer.performUpdate(root);
    };
    Server server(1, callback, 8080);

    while (1) {
        server.processReads();
    }

    return 0;
}
