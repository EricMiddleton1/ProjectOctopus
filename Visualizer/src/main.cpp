#include <iostream>
#include <string>
#include <thread>

#include <cmath>

#include "StripWindow.hpp"
#include "CloudServer.hpp"

#include "json/json.h"

using namespace std;

const std::string WINDOW_NAME = "ColorBot Visualizer";

const size_t PIXEL_WIDTH = 200, PIXEL_HEIGHT = 200;

const int PIXEL_CENTER = (PIXEL_WIDTH/2);
const double PIXELS_PER_METER = (20);

int main() {
	boost::asio::io_service ioService;
	boost::asio::io_service::work ioWork(ioService);

	StripWindow window(WINDOW_NAME, PIXEL_WIDTH, PIXEL_HEIGHT);

	double x = 0., y = 0., theta = 0.;

	CloudServer server(ioService, 8080, [&window, &x, &y, &theta](const std::string& msg) {
		Json::Value root = msg;
		Json::Reader reader;
		reader.parse(msg, root);

		std::cout << msg << std::endl;

		double dx = root["dx"].asDouble(), dy = root["dy"].asDouble(),
			phi = root["phi"].asDouble();

		auto jsonColor = root["color"];
		Color c(jsonColor[0].asInt(), jsonColor[1].asInt(), jsonColor[2].asInt());

/*
		std::cout << "\r\n[Info] dx=" << dx << ", dy=" << dy << ", phi="
			<< phi << ", color=" << c.toString() << std::endl;
*/
		theta += phi;
		x += dx*cos(theta) - dy*sin(theta);
		y += dy*cos(theta) + dx*sin(theta);
/*
		std::cout << "Pos=(" << x << ", " << y << "); theta=" << theta
			<< std::endl;
*/
		
		int pixelX = PIXEL_CENTER + x*PIXELS_PER_METER + 0.5,
			pixelY = PIXEL_CENTER + y*PIXELS_PER_METER + 0.5;

		if( (pixelX >= 0) && (pixelX < PIXEL_WIDTH) &&
			(pixelY >= 0) && (pixelY < PIXEL_HEIGHT) ) {
			std::cout << "[Info] (" << pixelX << ", " << pixelY << "): " << c.toString()
				<< std::endl;
			window.drawLED(pixelX, pixelY, c);
		}
		else {
			std::cout << "[Warning] Pixel coordinate out of bounds: (" << pixelX
				<< ", " << pixelY << ")" << std::endl;
		}

		return std::string{};
	});

	std::thread boostThread([&ioService]() {
		ioService.run();

		std::cout << "[Error] boostThread exit" << std::endl;
	});

	while(true) {
		window.windowUpdate();
	}

	return 0;
}
