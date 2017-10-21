#include <jsoncpp/json/value.h>

#include <mrpt/slam/CMetricMapBuilderRBPF.h>
#include <mrpt/obs/CActionRobotMovement2D.h>
#include <mrpt/maps/CColouredPointsMap.h>
class SLAMmer {
public:
    SLAMmer();

    void performUpdate(Json::Value root);

private:
    mrpt::slam::CMetricMapBuilderRBPF::TConstructionOptions rbpfMappingOptions;
};