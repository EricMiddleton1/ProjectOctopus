#include <iostream>

#include "SLAMmer.hpp"
#include <mrpt/maps/CMultiMetricMap.h>

using namespace mrpt::slam;
using namespace mrpt::bayes;
using namespace mrpt::obs;
using namespace mrpt::poses;
using namespace mrpt::maps;

SLAMmer::SLAMmer() {

    // Setup map
    rbpfMappingOptions.PF_options.PF_algorithm = CParticleFilter::pfAuxiliaryPFOptimal;
    rbpfMappingOptions.PF_options.sampleSize = 100;
    rbpfMappingOptions.insertionLinDistance = 0.03;
    rbpfMappingOptions.localizeLinDistance = 0.03;
    rbpfMappingOptions.insertionAngDistance = 1;
    rbpfMappingOptions.localizeAngDistance = 1;
    auto refl_map_init = new CReflectivityGridMap2D::TMapDefinition();
    refl_map_init->min_x = 0; refl_map_init->min_y = 0;
    refl_map_init->max_x = 1.5; refl_map_init->max_y = 1.5;
    refl_map_init->resolution = 0.05;
    // refl_map_init.insertionOpts.maxDistanceInsertion
    auto init_maps = TSetOfMetricMapInitializers();
    init_maps.push_back(TMetricMapInitializerPtr(refl_map_init));
    rbpfMappingOptions.mapsInitializers = init_maps;

    // rbpfMappingOptions.PF_options.resamplingMethod = CParticleFilter::prSystematic;

    // Create Rao-Blackweillized Particle Filter with options
    mapBuilder = CMetricMapBuilderRBPF(rbpfMappingOptions);

    // sf_frame = CSensoryFramePtr(new CSensoryFrame());
    // sf_frame = CSensoryFrame::Create();
    // action = CActionCollection::Create();

    // setup motion model
    motion_model.modelSelection = CActionRobotMovement2D::mmGaussian;
    motion_model.gaussianModel.a1 = 0; // meters/meter
    motion_model.gaussianModel.a2 = 0; // meters/degree
    motion_model.gaussianModel.a3 = 0; // degrees/meter
    motion_model.gaussianModel.a4 = 0; // degrees/degree
}


void SLAMmer::performUpdate(const Json::Value& root) {
    // movement info
    double dx, dy, dphi;
    // observation
    int r, g, b;
    // metadata
    int robot_id;
    uint64_t reading_time;

    try {
        dx = root.get("dx", 0).asDouble();
        dy = root.get("dy", 0).asDouble();
        dphi = root.get("dphi", 0).asDouble();

        Json::Value color = root["color"];
        r = color[0].asInt();
        g = color[1].asInt();
        b = color[2].asInt();

        reading_time = root.get("time", 0).asUInt64();
        robot_id = root.get("id", 0).asInt();
    }
    catch (std::exception& e) {
        std::cout << "Exception when reading JSON: " << e.what() << std::endl;
    }

    {
        // TODO: calculate standard devation
        motion_model.gaussianModel.minStdXY = 0.01;
        // Note: in degrees
        motion_model.gaussianModel.minStdPHI = 0.1;

        CActionCollectionPtr action = CActionCollection::Create();

        CActionRobotMovement2D actmov;
        CPose2D pose_change(dx, dy, (180.f / M_PI) * dphi);
        actmov.computeFromOdometry(pose_change, motion_model);
        // timestamp is in 100-nanosecond intervals
        actmov.timestamp = reading_time * 10;

        action->insert(actmov);

        CSensoryFramePtr sf_frame = CSensoryFrame::Create();
        // CObservationReflectivity* observation = new CObservationReflectivity();
        // observation->reflectivityLevel = (float)r / 255.f;
        // observation->sensorStdNoise = 0.05;

        // CObservationReflectivityPtr obs_ptr(new CObservationReflectivity());
        CObservationReflectivityPtr obs_ptr = CObservationReflectivity::Create();
        obs_ptr->reflectivityLevel = (float)r / 255.f;
        obs_ptr->sensorStdNoise = 0.05;
        obs_ptr->timestamp = reading_time * 10;
        sf_frame->insert(obs_ptr);


        // std::cout << "insertionLinDist: " << CMetricMapBuilderRBPF::insertionLinDistance << std::endl;
        mapBuilder.processActionObservation(*action, *sf_frame);

        CPose3D estimate_mean;
        CPose3DPDFPtr estimate_pdf = mapBuilder.getCurrentPoseEstimation();
        estimate_pdf->getMean(estimate_mean);
        std::cout << "Estimated position: " << estimate_mean.x() << ", " << estimate_mean.y() << std::endl;

        // sf_frame->clear();
        // action->clear();
    }
}

void SLAMmer::getEstimatedMap(mrpt::utils::CImage& map_img) {
    static int cnt = 0;
    const CMultiMetricMap* mostLikMap = mapBuilder.mapPDF.getCurrentMostLikelyMetricMap();
    // std::cout << mostLikMap->maps.size() << " maps" << std::endl;
    // if (mostLikMap->m_gridMaps.size() > 0) {
    if (mostLikMap->m_reflectivityMaps.size() > 0) {
        CReflectivityGridMap2DPtr map = mostLikMap->m_reflectivityMaps[0];
        map->getAsImage(map_img);
        // mapBuilder.drawCurrentEstimationToImage(&map_img);
        // map_img.saveToFile(std::string("./mapping_") + std::to_string(cnt) + ".png");
    }
    else {
        std::cout << "No reflectivity maps" << std::endl;
    }
    cnt++;;
}