#include <iostream>

#include "server.hpp"
#include "SLAMmer.hpp"

void server_callback(Json::Value root);


int main() {
    SLAMmer slammer;

    Server server(1, slammer.performUpdate, 8080);

    while (1) {
        server.processReads();
    }

    return 0;
}
