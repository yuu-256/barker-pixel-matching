#pragma once
#include <vector>
#include <array>

struct MSI_RGR_Data {
    using Vec2D = std::vector<std::vector<double>>;
    using Vec2DInt = std::vector<std::vector<int>>;
    using Vec3D = std::vector<std::vector<std::vector<double>>>;
    Vec3D radiance;
    Vec2D longitude;
    Vec2D latitude;
    Vec2D mu0;
    Vec2D phi0;
    Vec2DInt surface_type;
};

struct AC_CLP_Data {
    using Vec2D = std::vector<std::vector<double>>;
    using Vec2DInt = std::vector<std::vector<int>>;
    using Point = std::array<double, 2>;
    using Spectrum = std::array<double, 7>;

    Vec2D cloud_effective_radius1;
    Vec2D cloud_effective_radius2;
    Vec2D cloud_water_content1;
    Vec2D cloud_water_content2;
    Vec2DInt cloud_phase1;
    Vec2DInt cloud_phase2;
    Vec2DInt radar_lidar_flag;
    Vec2D height;

    std::vector<double> longitude;
    std::vector<double> latitude;

    std::vector<Spectrum> radiance;
};


struct AUX__2D_Data {
    using Vec2D = std::vector<std::vector<double>>;

    Vec2D ozoneMassMixingRatio;
    Vec2D pressure;
    Vec2D specificHumidity;
    Vec2D temperature;
    Vec2D height;

    std::vector<double> surfacePressure;
    std::vector<double> totalColumnOzone;
    std::vector<double> totalColumnWaterVapor;
    std::vector<int> day_night_flag;  // 1: day, 0: night
    std::vector<int> land_water_flag; // 1: land, 0: water
    std::vector<double> longitude;
    std::vector<double> latitude;
};
