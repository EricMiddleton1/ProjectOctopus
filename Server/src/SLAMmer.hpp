#include <vector>

#include <jsoncpp/json/value.h>

#include <mrpt/slam/CMetricMapBuilderRBPF.h>
#include <mrpt/obs/CActionRobotMovement2D.h>
#include <mrpt/obs/CObservationReflectivity.h>
#include <mrpt/utils/CImage.h>
#include <mrpt/utils/CFileOutputStream.h>

class SLAMmer {
public:
    SLAMmer(mrpt::slam::CMetricMapBuilderRBPF::TConstructionOptions& rbpfMappingOptions);

    void performUpdate(const Json::Value& root);

    void getEstimatedMap(mrpt::utils::CImage& red_img, mrpt::utils::CImage& green_img, mrpt::utils::CImage& blue_img);

private:
    mrpt::obs::CActionRobotMovement2D::TMotionModelOptions motion_model;

    // mrpt::obs::CSensoryFramePtr sf_frame;
    // mrpt::obs::CActionCollectionPtr action;

    std::vector<uint64_t> last_update_times;
    
    mrpt::slam::CMetricMapBuilderRBPF mapBuilder;

    mrpt::utils::CFileOutputStream log_f;
};