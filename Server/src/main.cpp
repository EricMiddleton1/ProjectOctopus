#include <iostream>
#include <functional>

#include "server.hpp"
#include "SLAMmer.hpp"
#include <SFML/Graphics.hpp>
#include <mrpt/utils/CImage.h>
#include <mrpt/bayes/CParticleFilter.h>
#include <mrpt/utils/CConfigFile.h>

// void server_callback(Json::Value root);

using namespace mrpt::maps;
using namespace mrpt::poses;
using namespace mrpt::obs;
using namespace mrpt::utils;

void drawMap(const CImage& red_map_img, const CImage& green_map_img, const CImage& blue_map_img, sf::Image& image, const int img_size, const float map_size);

int main() {
    const int img_size = 256;
    sf::RenderWindow window(sf::VideoMode(img_size, img_size), "window");
    sf::Sprite sprite;
    sf::Texture texture;
    texture.create(img_size, img_size);
    sf::Image image;
    image.create(img_size, img_size, sf::Color(255, 255, 0));
    sprite.setTexture(texture);
    
    float map_size = 2;


    // Setup map
    mrpt::slam::CMetricMapBuilderRBPF::TConstructionOptions rbpfMappingOptions;
    // rbpfMappingOptions.PF_options.PF_algorithm = mrpt::bayes::CParticleFilter::pfAuxiliaryPFOptimal;
    // // rbpfMappingOptions.PF_options.sampleSize = 10;
    // // rbpfMappingOptions.PF_options.resamplingMethod = mrpt::bayes::CParticleFilter::prSystematic;
    // rbpfMappingOptions.insertionLinDistance = 0;
    // rbpfMappingOptions.insertionAngDistance = 0;
    // rbpfMappingOptions.localizeLinDistance = 0.08;
    // rbpfMappingOptions.localizeAngDistance = 1;
    // auto refl_map_init = new CReflectivityGridMap2D::TMapDefinition();
    // refl_map_init->min_x = 0; refl_map_init->min_y = 0;
    // refl_map_init->max_x = 1.5; refl_map_init->max_y = 1.5;
    // refl_map_init->resolution = 0.05;
    // // refl_map_init.insertionOpts.maxDistanceInsertion
    // auto init_maps = TSetOfMetricMapInitializers();
    // init_maps.push_back(TMetricMapInitializerPtr(refl_map_init));
    // rbpfMappingOptions.mapsInitializers = init_maps;
    mrpt::utils::CConfigFile config_file("config.ini");
    rbpfMappingOptions.loadFromConfigFile(config_file, "MappingApplication");
    rbpfMappingOptions.dumpToConsole();
    SLAMmer slammer(rbpfMappingOptions);

    auto callback = [&slammer] (const Json::Value& root) {
        slammer.performUpdate(root);
    };
    Server server(1, callback, 8080);

    CImage red_map_img, green_map_img, blue_map_img;

    // while (1) {
    while (window.isOpen()) {
        sf::Event event;
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        server.processReads();

        slammer.getEstimatedMap(red_map_img, green_map_img, blue_map_img);
        drawMap(red_map_img, green_map_img, blue_map_img, image, img_size, map_size);
        window.clear();
        texture.update(image);
        window.draw(sprite);
        window.display();

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

void drawMap(const CImage& red_map_img, const CImage& green_map_img, const CImage& blue_map_img, sf::Image& image, const int img_size, const float map_size) {
    // Assuming red, green, and blue maps are same size
    for (size_t u = 0; u < red_map_img.getWidth(); ++u) {
        for (size_t v = 0; v < red_map_img.getHeight(); ++v) {
            unsigned char red = *red_map_img(u, v);
            unsigned char green = *green_map_img(u, v);
            unsigned char blue = *blue_map_img(u, v);
            image.setPixel(u*2, v*2, sf::Color(red, green, blue));
            image.setPixel(u*2, v*2+1, sf::Color(red, green, blue));
            image.setPixel(u*2+1, v*2, sf::Color(red, green, blue));
            image.setPixel(u*2+1, v*2+1, sf::Color(red, green, blue));
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