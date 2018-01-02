#include <iostream>

#include "SLAMmer.hpp"
#include <mrpt/maps/CMultiMetricMap.h>
#include <mrpt/random.h>

using namespace mrpt::slam;
using namespace mrpt::bayes;
using namespace mrpt::obs;
using namespace mrpt::poses;
using namespace mrpt::maps;



SLAMmer::SLAMmer(CMetricMapBuilderRBPF::TConstructionOptions& rbpfMappingOptions) : mapBuilder(rbpfMappingOptions), log_f("data.rawlog") {


    // rbpfMappingOptions.PF_options.resamplingMethod = CParticleFilter::prSystematic;

    // Create Rao-Blackweillized Particle Filter with options
    // mapBuilder = CMetricMapBuilderRBPF(rbpfMappingOptions);

    // sf_frame = CSensoryFramePtr(new CSensoryFrame());
    // sf_frame = CSensoryFrame::Create();
    // action = CActionCollection::Create();

    // setup motion model
    motion_model.modelSelection = CActionRobotMovement2D::mmGaussian;
    motion_model.gaussianModel.a1 = 0; // meters/meter
    motion_model.gaussianModel.a2 = 0; // meters/degree
    motion_model.gaussianModel.a3 = 0; // degrees/meter
    motion_model.gaussianModel.a4 = 0; // degrees/degree
    motion_model.gaussianModel.minStdXY = 0.005; // meters
    motion_model.gaussianModel.minStdPHI = 1; // degrees
    
    mapBuilder.options.enableMapUpdating = true;
    mapBuilder.options.debugForceInsertion = false;
    mrpt::random::Randomize();

    // Initialize map
    // CSimpleMap       dummySimpleMap;
    // CPosePDFGaussian startPose;


    // startPose.mean.x(0);
    // startPose.mean.y(0);
    // startPose.mean.phi(0.01);
    // startPose.cov.setZero();

    // // mrpt::maps::COccupancyGridMap2D gridmap;
    // // {
    // //     mrpt::utils::CFileGZInputStream f(METRIC_MAP_CONTINUATION_GRIDMAP_FILE);
    // //     f >> gridmap;
    // // }

    // mapBuilder.initialize(dummySimpleMap, &startPose);

    
	// 	for (CMultiMetricMapPDF::CParticleList::iterator it=mapBuilder.mapPDF.m_particles.begin();it!=mapBuilder.mapPDF.m_particles.end();++it) {
	// 		CRBPFParticleData* part_d = it->d.get();
	// 		CMultiMetricMap &mmap = part_d->mapTillNow;
	// 		mrpt::maps::CReflectivityGridMap2DPtr it_grid = mmap.getMapByClass<mrpt::maps::CReflectivityGridMap2D>();
	// 		ASSERTMSG_(it_grid.present(), "No gridmap in multimetric map definition, but metric map continuation was set (!)" );
	// 		// it_grid->copyMapContentFrom( gridmap );
	// 	}

    // mapBuilder.initialize();
    mapBuilder.setVerbosityLevel(  mrpt::utils::LVL_INFO );  // default value: as loaded from config file
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

    long utc_timestamp = reading_time * 10;
    // float sensor_std = 0.01;
    float sensor_std = 0.01;

    {
        // TODO: calculate standard devation
        motion_model.gaussianModel.minStdXY = 0.005;
        // Note: in degrees
        motion_model.gaussianModel.minStdPHI = 0.01;

        CActionCollection::Ptr action = CActionCollection::Create();

        CActionRobotMovement2D actmov;
        CPose2D pose_change(dx, dy, dphi);
        actmov.computeFromOdometry(pose_change, motion_model);
        // timestamp is in 100-nanosecond intervals
        actmov.timestamp = utc_timestamp;

        action->insert(actmov);

        CSensoryFrame::Ptr sf_frame = CSensoryFrame::Create();
        // CObservationReflectivity* observation = new CObservationReflectivity();
        // observation->reflectivityLevel = (float)r / 255.f;
        // observation->sensorStdNoise = 0.05;


        // CObservationReflectivityPtr obs_ptr(new CObservationReflectivity());
        CObservationReflectivity::Ptr obs_red = CObservationReflectivity::Create();
        CObservationReflectivity::Ptr obs_green = CObservationReflectivity::Create();
        CObservationReflectivity::Ptr obs_blue = CObservationReflectivity::Create();
        obs_red->reflectivityLevel = (float)r / 255.f;
        obs_green->reflectivityLevel = (float)g / 255.f;
        obs_blue->reflectivityLevel = (float)b / 255.f;
        obs_red->channel = 0;
        obs_green->channel = 1;
        obs_blue->channel = 2;
        obs_red->sensorStdNoise = obs_green->sensorStdNoise = obs_blue->sensorStdNoise = sensor_std;
        obs_red->timestamp = obs_green->timestamp = obs_blue->timestamp = utc_timestamp;

        sf_frame->insert(obs_red);
        sf_frame->insert(obs_green);
        sf_frame->insert(obs_blue);

        static int count = 0;
        if (count == 100) {
            log_f.close();
        }
        else if (count < 100) {
            log_f << *action << *sf_frame;
        }
        count += 1;

        // std::cout << "insertionLinDist: " << CMetricMapBuilderRBPF::insertionLinDistance << std::endl;
        mapBuilder.processActionObservation(*action, *sf_frame);

        CPose3D estimate_mean;
        CPose3DPDF::Ptr estimate_pdf = mapBuilder.getCurrentPoseEstimation();
        estimate_pdf->getMean(estimate_mean);
        std::cout << "Estimated position: " << estimate_mean.x() << ", " << estimate_mean.y() << std::endl;

        // sf_frame->clear();
        // action->clear();
    }
}

void SLAMmer::getEstimatedMap(mrpt::utils::CImage& red_img, mrpt::utils::CImage& green_img, mrpt::utils::CImage& blue_img) {
    static int cnt = 0;
    const CMultiMetricMap* mostLikMap = mapBuilder.mapPDF.getCurrentMostLikelyMetricMap();
    // std::cout << mostLikMap->maps.size() << " maps" << std::endl;
    // if (mostLikMap->m_gridMaps.size() > 0) {
    if (mostLikMap->m_reflectivityMaps.size() == 3) {
        CReflectivityGridMap2D::Ptr red_map = mostLikMap->m_reflectivityMaps[0];
        CReflectivityGridMap2D::Ptr green_map = mostLikMap->m_reflectivityMaps[1];
        CReflectivityGridMap2D::Ptr blue_map = mostLikMap->m_reflectivityMaps[2];
        red_map->getAsImage(red_img);
        green_map->getAsImage(green_img);
        blue_map->getAsImage(blue_img);
        // mapBuilder.drawCurrentEstimationToImage(&map_img);
        // map_img.saveToFile(std::string("./mapping_") + std::to_string(cnt) + ".png");
    }
    else {
        std::cout << "Not the right number of reflectivity maps" << std::endl;
    }
    cnt++;
}