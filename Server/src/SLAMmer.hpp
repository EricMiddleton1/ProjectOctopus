#include <vector>

#include <jsoncpp/json/value.h>

#include <mrpt/slam/CMetricMapBuilderRBPF.h>
#include <mrpt/obs/CActionRobotMovement2D.h>
#include <mrpt/obs/CObservationReflectivity.h>
#include <mrpt/utils/CImage.h>

class SLAMmer {
public:
    SLAMmer();

    void performUpdate(const Json::Value& root);

    void getEstimatedMap(mrpt::utils::CImage& map_img);

private:
    mrpt::slam::CMetricMapBuilderRBPF::TConstructionOptions rbpfMappingOptions;
    mrpt::obs::CActionRobotMovement2D::TMotionModelOptions motion_model;

    // mrpt::obs::CSensoryFramePtr sf_frame;
    // mrpt::obs::CActionCollectionPtr action;

    std::vector<uint64_t> last_update_times;
    
    mrpt::slam::CMetricMapBuilderRBPF mapBuilder;
};