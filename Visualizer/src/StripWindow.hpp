#pragma once

#include <mutex>
#include <string>
#include <tuple>

#include <SFML/Graphics.hpp>
#include "Color.hpp"

class StripWindow
{
public:
	StripWindow(const std::string& name, size_t width, size_t height);

	//SFML has a problem with the window event loop
	//running in any thread other than the main thread.
	//So, you have to call this often from the main thread
	bool windowUpdate();
	
	void clear();
	void drawLED(size_t, size_t, const Color& c);

private:
	static const int LED_BORDER = 0;
	static const int LED_SIZE = 4;
	static const int LED_OUTLINE_THICKNESS = 0;

	std::vector<std::tuple<size_t, size_t, sf::Color>> toDraw;
	std::mutex drawMutex;

	sf::RenderWindow window;
	size_t width, height;
};
