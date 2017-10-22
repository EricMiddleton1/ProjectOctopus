#pragma once

#include <string>
#include <array>
#include <cstdint>

#include <boost/asio.hpp>

class CloudServer {
public:
	using ReceiveHandler = std::function<std::string(const std::string& msg)>;

	CloudServer(boost::asio::io_service& ioService, uint16_t port, const ReceiveHandler& handler);

private:
	void startAccept();
	void startListen();

	static bool parseMessage(std::string& buffer, std::string& msg);

	boost::asio::io_service& ioService;
	
	boost::asio::ip::tcp::endpoint endpoint, clientEndpoint;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
	std::array<uint8_t, 512> readBuffer;
	
	std::string msgBuffer;

	ReceiveHandler handler;
};
