#include <iostream>
#include <functional>

#include "server.hpp"
#include "SLAMmer.hpp"
#include <SFML/Graphics.hpp>
#include <mrpt/utils/CImage.h>

// void server_callback(Json::Value root);

using namespace mrpt::maps;
using namespace mrpt::poses;
using namespace mrpt::obs;
using namespace mrpt::utils;

void drawMap(const CImage& map_img, sf::Image& image, const int img_size, const float map_size);

int main() {
    const int img_size = 512;
    sf::RenderWindow window(sf::VideoMode(img_size, img_size), "window");
    sf::Sprite sprite;
    sf::Texture texture;
    texture.create(img_size, img_size);
    sf::Image image;
    image.create(img_size, img_size, sf::Color(255, 255, 0));
    sprite.setTexture(texture);
    
    float map_size = 1.5;


    SLAMmer slammer;

    auto callback = [&slammer] (const Json::Value& root) {
        slammer.performUpdate(root);
    };
    Server server(1, callback, 8080);

    CImage map_img;

    // while (1) {
    while (window.isOpen()) {
        sf::Event event;
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        server.processReads();

        slammer.getEstimatedMap(map_img);
        drawMap(map_img, image, img_size, map_size);
        window.clear();
        texture.update(image);
        window.draw(sprite);
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

void drawMap(const CImage& map_img, sf::Image& image, const int img_size, const float map_size) {
    for (int u = 0; u < map_img.getWidth(); ++u) {
        for (int v = 0; v < map_img.getHeight(); ++v) {
            unsigned char intensity = *map_img(u, v);
            image.setPixel(u, v, sf::Color(intensity, intensity, intensity));
        }
    }
    // CPose3DPDFPtr pose_pdf;
    // CSensoryFramePtr s_frame;
    // CPose3D pose;
    // for (size_t i = 0; i < map.size(); ++i) {
    //     map.get(i, pose_pdf, s_frame);
    //     pose_pdf->getMean(pose);
    //     int pixel_x = (pose.x() / map_size) * img_size;
    //     int pixel_y = (pose.y() / map_size) * img_size;
    //     pixel_x = std::min(img_size - 1, std::max(0, pixel_x));
    //     pixel_y = std::min(img_size - 1, std::max(0, pixel_y));
    //     std::cout << "pose at " << pose.x() << ", " << pose.y() << std::endl;

    //     for (const CObservationPtr obs : *s_frame) {
    //         sf::CircleShape color_pt(0.05 * img_size);
    //         color_pt.setFillColor(sf::Color(255, 0, 0));
    //         color_pt.setPosition(pixel_x * img_size, pixel_y * img_size);
    //         window.draw(color_pt);
    //     }
    // }
}