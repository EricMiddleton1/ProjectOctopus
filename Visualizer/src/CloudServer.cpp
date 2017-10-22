#include "CloudServer.hpp"

#include <algorithm>
#include <iostream>

using namespace boost::asio;

CloudServer::CloudServer(io_service& _ioService, uint16_t _port,
	const ReceiveHandler& _handler)
	:	ioService{_ioService}
	,	endpoint{ip::tcp::v4(), _port}
	, acceptor{ioService, endpoint}
	,	socket{ioService}
	,	handler{_handler} {
	
	ioService.post([this]() {
		startAccept();
	});
}

void CloudServer::startAccept() {
	std::cout << "[Info] CloudServer: Starting Accept" << std::endl;

	acceptor.async_accept(socket, clientEndpoint, [this](const boost::system::error_code& ec) {
		if(ec) {
			std::cout << "[Error] CloudServer::handleAccept: " << ec.message() << std::endl;
			startAccept();
		}
		else {
			std::cout << "[Info] CoudServer: Client connected" << std::endl;
			startListen();
		}
	});
}

void CloudServer::startListen() {
	auto n = boost::asio::read(socket, boost::asio::buffer(readBuffer),
		boost::asio::transfer_exactly(4));
	
	int toRead = readBuffer[0];
	std::cout << "[Info] CloudServer: JSON with length " << toRead << std::endl;

	n = boost::asio::read(socket, boost::asio::buffer(readBuffer),
		boost::asio::transfer_exactly(toRead));

	handler({readBuffer.begin(), readBuffer.begin() + toRead});

	ioService.post([this]() {
		startListen();
	});
}

bool CloudServer::parseMessage(std::string& buffer, std::string& msg) {
	const std::string endToken{"\r\n\r\n"};

	auto itr = std::search(buffer.begin(), buffer.end(),
		endToken.begin(), endToken.end());

	if(itr == buffer.end()) {
		return false;
	}
	else {
		msg = std::string(buffer.begin(), itr);
		buffer.erase(buffer.begin(), itr + endToken.length());

		return true;
	}
}
