#include <iostream>

#include "SLAMmer.hpp"

using namespace mrpt::slam;
using namespace mrpt::bayes;
using namespace mrpt::obs;

SLAMmer::SLAMmer() {

    // Setup map
    rbpfMappingOptions.PF_options.PF_algorithm = CParticleFilter::pfAuxiliaryPFOptimal;
    rbpfMappingOptions.PF_options.sampleSize = 100;
    // rbpfMappingOptions.PF_options.resamplingMethod = CParticleFilter::prSystematic;

    
    CMetricMapBuilderRBPF mapBuilder(rbpfMappingOptions);

    CActionRobotMovement2D::TMotionModelOptions motion_model;
    motion_model.modelSelection = CActionRobotMovement2D::mmGaussian;
    motion_model.gaussianModel.a1 = 0; // meters/meter
    motion_model.gaussianModel.a2 = 0; // meters/degree
    motion_model.gaussianModel.a3 = 0; // degrees/meter
    motion_model.gaussianModel.a4 = 0; // degrees/degree
}


void  SLAMmer::performUpdate(Json::Value root) {
    std::cout << "Got a callback" << std::endl;
    int val;
    try {
        val = root.get("thing", -1).asInt();
    }
    catch (std::exception& e) {
        std::cout << "Exception when reading JSON: " << e.what() << std::endl;
    }
    std::cout << "thing = " << val << std::endl;
}