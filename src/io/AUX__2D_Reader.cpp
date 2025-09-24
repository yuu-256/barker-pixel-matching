#include "AUX__2D_Reader.hpp"
#include <highfive/H5File.hpp>
#include <iostream>

std::unique_ptr<AUX__2D_Data> AUX__2D_Reader::read(const std::string& filepath) {
    using namespace HighFive;
    std::cout << "[AUX__2D_Reader] Reading file: " << filepath << std::endl;

    File file(filepath, File::ReadOnly);
    auto aux2d_data = std::make_unique<AUX__2D_Data>();

    // Coordinates
    file.getDataSet("ScienceData/Geo/longitude").read(aux2d_data->longitude); // [N]
    file.getDataSet("ScienceData/Geo/latitude").read(aux2d_data->latitude);   // [N]
    size_t N = aux2d_data->longitude.size();
    std::cout << "[AUX__2D_Reader] Geo points: " << N << std::endl;

    // Science data
    file.getDataSet("ScienceData/Data/ozoneMassMixingRatio").read(aux2d_data->ozoneMassMixingRatio);
    file.getDataSet("ScienceData/Data/pressure").read(aux2d_data->pressure);
    file.getDataSet("ScienceData/Data/specificHumidity").read(aux2d_data->specificHumidity);
    file.getDataSet("ScienceData/Data/temperature").read(aux2d_data->temperature);
    file.getDataSet("ScienceData/Data/surfacePressure").read(aux2d_data->surfacePressure);
    file.getDataSet("ScienceData/Data/totalColumnOzone").read(aux2d_data->totalColumnOzone);
    file.getDataSet("ScienceData/Data/totalColumnWaterVapour").read(aux2d_data->totalColumnWaterVapor);
    file.getDataSet("ScienceData/Geo/day_night_flag").read(aux2d_data->day_night_flag);
    file.getDataSet("ScienceData/Geo/land_water_flag").read(aux2d_data->land_water_flag);
    file.getDataSet("ScienceData/Geo/height").read(aux2d_data->height);

    return aux2d_data;
}
