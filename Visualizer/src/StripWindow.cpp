#include "StripWindow.hpp"

#include <iostream>


StripWindow::StripWindow(const std::string& _name, size_t _width, size_t _height)
	:	window(sf::VideoMode(_width * (LED_BORDER + LED_SIZE) + LED_BORDER,
		_height * (LED_BORDER + LED_SIZE) + LED_BORDER), _name) 
	,	width{_width}
	,	height{_height} {
}

bool StripWindow::windowUpdate() {

	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed) {
			window.close();
			return false;
		}
	}

	std::unique_lock<std::mutex> drawLock(drawMutex);

	if(toDraw.size() > 0) {
		for(const auto& led : toDraw) {
			size_t xPos = (LED_SIZE + LED_BORDER)*std::get<0>(led) + LED_BORDER,
				yPos = (LED_SIZE + LED_BORDER)*std::get<1>(led) + LED_BORDER;


			sf::RectangleShape ledShape(sf::Vector2f(LED_SIZE, LED_SIZE));

			ledShape.setPosition(xPos, yPos);
			ledShape.setOutlineColor(sf::Color(64, 64, 64));
			ledShape.setOutlineThickness(LED_OUTLINE_THICKNESS);
			ledShape.setFillColor(std::get<2>(led));
			
			window.draw(ledShape);
		}
		toDraw.clear();

		window.display();
	}

	return true;
}

void StripWindow::clear() {
	std::unique_lock<std::mutex> drawLock(drawMutex);
	window.clear();
}

void StripWindow::drawLED(size_t x, size_t y, const Color& c) {
	std::unique_lock<std::mutex> drawLock(drawMutex);
	toDraw.emplace_back(x, y, sf::Color(c.getRed(), c.getGreen(), c.getBlue()));
}
