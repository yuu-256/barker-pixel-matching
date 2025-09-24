#include "MSI_RGR_Reader.hpp"
#include <highfive/H5File.hpp>
#include <iostream>

std::unique_ptr<MSI_RGR_Data> MSI_Reader::read(const std::string& filepath) {
    using namespace HighFive;
    std::cout << "[MSI_Reader] Reading file: " << filepath << std::endl;

    File file(filepath, File::ReadOnly);
    auto msi_data = std::make_unique<MSI_RGR_Data>();

    // Coordinates
    file.getDataSet("ScienceData/longitude").read(msi_data->longitude); // [H][W]
    file.getDataSet("ScienceData/latitude").read(msi_data->latitude);   // [H][W]

    // Radiance
    auto dataset = file.getDataSet("ScienceData/pixel_values");
    std::vector<size_t> dims = dataset.getDimensions();
    std::cout << "[MSI_Reader] Radiance dimensions: " << dims[0] << "," << dims[1] << "," << dims[2] << std::endl;

    std::vector<std::vector<std::vector<double>>> radiance_3d;
    dataset.read(radiance_3d);

    size_t B = dims[0]; // Number of bands
    size_t H = dims[1]; // Height
    size_t W = dims[2]; // Width
    msi_data->radiance.resize(H, std::vector<std::vector<double>>(W, std::vector<double>(B)));
    for (size_t i = 0; i < H; ++i) {
        for (size_t j = 0; j < W; ++j) {
            for (size_t b = 0; b < B; ++b) {
                msi_data->radiance[i][j][b] = radiance_3d[b][i][j];
            }
        }
    }
    std::cout << "[MSI_Reader] Sample radiance[37][21][2]: " << msi_data->radiance[37][21][2] << std::endl;

    file.getDataSet("ScienceData/solar_elevation_angle").read(msi_data->mu0);
    file.getDataSet("ScienceData/solar_azimuth_angle").read(msi_data->phi0);
    file.getDataSet("ScienceData/land_flag").read(msi_data->surface_type);

    std::cout << "[MSI_Reader] mu0 shape: " << msi_data->mu0.size() << std::endl;
    std::cout << "[MSI_Reader] phi0 shape: " << msi_data->phi0.size() << std::endl;
    std::cout << "[MSI_Reader] surface_type shape: " << msi_data->surface_type.size() << std::endl;

    return msi_data;
}
