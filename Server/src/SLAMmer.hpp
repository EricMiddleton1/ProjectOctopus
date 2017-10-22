#include <vector>

#include <jsoncpp/json/value.h>

#include <mrpt/slam/CMetricMapBuilderRBPF.h>
#include <mrpt/obs/CActionRobotMovement2D.h>
#include <mrpt/obs/CObservationReflectivity.h>

class SLAMmer {
public:
    SLAMmer();

    void performUpdate(const Json::Value& root);

private:
    mrpt::slam::CMetricMapBuilderRBPF::TConstructionOptions rbpfMappingOptions;
    mrpt::obs::CActionRobotMovement2D::TMotionModelOptions motion_model;

    std::vector<uint64_t> last_update_times;
    
    mrpt::slam::CMetricMapBuilderRBPF mapBuilder;
};