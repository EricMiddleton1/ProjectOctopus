#include <mrpt/utils/CClientTCPSocket.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include <vector>
#include <string>
#include <cassert>

#include "server.hpp"
using namespace mrpt::utils;

RobotConnection::RobotConnection(CClientTCPSocket* sock) {
    socket = sock;
}

RobotConnection::~RobotConnection() {
    delete socket;
}

void RobotConnection::processReads(SCb callback) {
    // Enough data for at least a packet header
    while (socket->getReadPendingBytes() >= sizeof(size_remaining)) {
        // Start of a new packet
        if (size_remaining == 0) {
            // Guaranteed to have enough data for size header if here
            socket->ReadBuffer(&size_remaining, sizeof(size_remaining));
            size_read = 0;
            unprocessed_data.reserve(size_remaining);
        }
        // Still have data left in this packet
        if (size_remaining) {
            // don't read beyond the end of the packet
            size_t to_read = std::min<uint32_t>(size_remaining, socket->getReadPendingBytes());
            size_t actual_read = socket->ReadBufferImmediate(&unprocessed_data[size_read], to_read);
            size_read += actual_read;
            size_remaining -= actual_read;
            assert(actual_read == to_read);

            // If read the whole packet
            if (size_remaining == 0) {
                // Convert to JSON
                Json::Reader reader;
                Json::Value json_root;
                bool parsing_ok = reader.parse(&unprocessed_data[0], &unprocessed_data[size_read], json_root);
                if (parsing_ok) {
                    callback(json_root);
                }
                else {
                    std::cout << "Failed to parse JSON: " << std::endl << std::string(unprocessed_data.begin(), unprocessed_data.end()) << std::endl;
                }
            }
        }
    }
}

void RobotConnection::write(const Json::Value& root) {
    Json::FastWriter writer;
    std::string raw_data = writer.write(root);
    socket->WriteBuffer(raw_data.c_str(), raw_data.length());
}

Server::Server(size_t n_robots, const SCb callback, unsigned short listenPort, std::string ipAddr) : serverSock(listenPort, ipAddr) {
    cb_func = callback;
    std::cout << "Waiting for robots to connect..." << std::endl;
    while (robots.size() < n_robots) {
        CClientTCPSocket* req_client = serverSock.accept();
        RobotConnection robot(req_client);
        robots.push_back(robot);
    }
    std::cout << "All robots connected" << std::endl;

    // start write thread
    write_thread = std::thread(&Server::write_loop, this);
}

int Server::write(const Json::Value data, size_t robot_id) {
    {
        std::lock_guard<std::mutex> lock(send_q_lock);
        send_q.push(std::make_pair(data, &robots[robot_id]));
        pending_outgoing++;
    }
}

void Server::processReads() {
    for (RobotConnection& robot : robots) {
        robot.processReads(cb_func);
    }
}

void Server::write_loop() {
    while (1) {
        while (pending_outgoing > 0) {
            // Get a copy of the message to send, safely
            std::pair<Json::Value, RobotConnection*> to_send;
            {
                std::lock_guard<std::mutex> lock(send_q_lock);
                to_send = send_q.front();
                send_q.pop();
                pending_outgoing--;
            }
            (to_send.second)->write(to_send.first);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}