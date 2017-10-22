#include <iostream>

#include "SLAMmer.hpp"

using namespace mrpt::slam;
using namespace mrpt::bayes;
using namespace mrpt::obs;
using namespace mrpt::poses;

SLAMmer::SLAMmer() {

    // Setup map
    rbpfMappingOptions.PF_options.PF_algorithm = CParticleFilter::pfAuxiliaryPFOptimal;
    rbpfMappingOptions.PF_options.sampleSize = 100;
    // rbpfMappingOptions.PF_options.resamplingMethod = CParticleFilter::prSystematic;

    // Create Rao-Blackweillized Particle Filter with options
    mapBuilder = CMetricMapBuilderRBPF(rbpfMappingOptions);

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
    uint64_t time;

    try {
        dx = root.get("dx", 0).asDouble();
        dy = root.get("dy", 0).asDouble();
        dphi = root.get("dphi", 0).asDouble();

        Json::Value color = root["color"];
        r = color[0].asInt();
        g = color[1].asInt();
        b = color[2].asInt();

        time = root.get("time", 0).asUInt64();
        robot_id = root.get("id", 0).asInt();
    }
    catch (std::exception& e) {
        std::cout << "Exception when reading JSON: " << e.what() << std::endl;
    }

    // TODO: calculate standard devation
    motion_model.gaussianModel.minStdXY = 0.1;
    // Note: in degrees
    motion_model.gaussianModel.minStdPHI = 0.1;


    // TODO: Figure out TicTac
    CActionCollectionPtr act = CActionCollection::Create();

    CActionRobotMovement2D actmov;
    CPose2D pose_change(dx, dy, dphi);
    actmov.computeFromOdometry(pose_change, motion_model);
    // timestamp is in 100-nanosecond intervals
    actmov.timestamp = time * 10;

    act->insert(actmov);

    CSensoryFramePtr s_frame = CSensoryFrame::Create();
    CObservationReflectivity* observation = new CObservationReflectivity();
    observation->reflectivityLevel = (float)r / 255.f;
    observation->sensorStdNoise = 0.05;

    s_frame->insert(CObservationReflectivityPtr(observation));

    mapBuilder.processActionObservation(*act, *s_frame);

    CPose3D estimate_mean;
    CPose3DPDFPtr estimate_pdf = mapBuilder.getCurrentPoseEstimation();
    estimate_pdf->getMean(estimate_mean);
    std::cout << "Estimated position: " << estimate_mean.x() << ", " << estimate_mean.y() << std::endl;

}