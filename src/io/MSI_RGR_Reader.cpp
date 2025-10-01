#include "MSI_RGR_Reader.hpp"
#include <highfive/H5File.hpp>
#include <iostream>

std::unique_ptr<MSI_RGR_Data> MSI_RGR_Reader::read(const std::string& filepath) {
    using namespace HighFive;

    File file(filepath, File::ReadOnly);
    auto msi_rgr_data = std::make_unique<MSI_RGR_Data>();

    // Coordinates
    auto rad_ds = file.getDataSet("ScienceData/pixel_values");
    std::vector<size_t> dims = rad_ds.getSpace().getDimensions();
    size_t B = dims[0];
    size_t H = dims[1];
    size_t W = dims[2];

    msi_rgr_data->radiance      = Array3D(B, H, W);
    msi_rgr_data->longitude     = Array2D(H, W);
    msi_rgr_data->latitude      = Array2D(H, W);
    msi_rgr_data->mu0           = Array2D(H, W);
    msi_rgr_data->phi0          = Array2D(H, W);
    msi_rgr_data->surface_type  = Array2Dint(H, W);

    // Read datasets
    rad_ds.read_raw(msi_rgr_data->radiance.data.data(), HighFive::AtomicType<double>());
    file.getDataSet("ScienceData/longitude")
        .read_raw(msi_rgr_data->longitude.data.data(), HighFive::AtomicType<double>());
    file.getDataSet("ScienceData/latitude")
        .read_raw(msi_rgr_data->latitude.data.data(), HighFive::AtomicType<double>());
    file.getDataSet("ScienceData/solar_elevation_angle")
        .read_raw(msi_rgr_data->mu0.data.data(), HighFive::AtomicType<double>());
    file.getDataSet("ScienceData/solar_azimuth_angle")
        .read_raw(msi_rgr_data->phi0.data.data(), HighFive::AtomicType<double>());
    file.getDataSet("ScienceData/land_flag")
        .read_raw(msi_rgr_data->surface_type.data.data(), HighFive::AtomicType<int>());

    return msi_rgr_data;
}
