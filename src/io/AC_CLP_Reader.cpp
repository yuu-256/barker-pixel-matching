#include "AC_CLP_Reader.hpp"
#include <highfive/H5File.hpp>
#include <iostream>

std::unique_ptr<AC_CLP_Data> AC_CLP_Reader::read(const std::string& filepath) {
    using namespace HighFive;
    std::cout << "[AC_CLP_Reader] Reading file: " << filepath << std::endl;

    File file(filepath, File::ReadOnly);
    auto acclp_data = std::make_unique<AC_CLP_Data>();

    // Coordinates
    file.getDataSet("ScienceData/Geo/longitude").read(acclp_data->longitude); // [N]
    file.getDataSet("ScienceData/Geo/latitude").read(acclp_data->latitude);   // [N]
    size_t N = acclp_data->longitude.size();
    std::cout << "[AC_CLP_Reader] Geo points: " << N << std::endl;

    // Science data
    file.getDataSet("ScienceData/Data/cloud_effective_radius1_1km").read(acclp_data->cloud_effective_radius1);
    file.getDataSet("ScienceData/Data/cloud_effective_radius2_1km").read(acclp_data->cloud_effective_radius2);
    file.getDataSet("ScienceData/Data/cloud_water_content1_1km").read(acclp_data->cloud_water_content1);
    file.getDataSet("ScienceData/Data/cloud_water_content2_1km").read(acclp_data->cloud_water_content2);
    file.getDataSet("ScienceData/Data/cloud_phase1_1km").read(acclp_data->cloud_phase1);
    file.getDataSet("ScienceData/Data/cloud_phase2_1km").read(acclp_data->cloud_phase2);
    file.getDataSet("ScienceData/Data/radar_lider_flag_1km").read(acclp_data->radar_lidar_flag);
    file.getDataSet("ScienceData/Geo/height").read(acclp_data->height);
    
    size_t K = acclp_data->height.size();
    std::cout << "[AC_CLP_Reader] Vertical levels per point: " << K << std::endl;
    std::cout << "[AC_CLP_Reader] Cloud effective radius1[619][194] : " << acclp_data->cloud_effective_radius1[619][194] << std::endl;
    std::cout << "[AC_CLP_Reader] Sample height[0][0] : " << acclp_data->height[0][0] << std::endl;
    std::cout << "[AC_CLP_Reader] AC_CLP reading completed." << std::endl;

    return acclp_data;
}
