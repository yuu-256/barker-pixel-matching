#pragma once
#include <vector>
#include <array>

struct Array1D {
    std::vector<double> data;
    size_t n;
    Array1D(size_t n_) : data(n_), n(n_) {}
    inline double& operator()(size_t i) {
        return data[i];
    }
    inline const double& operator()(size_t i) const {
        return data[i];
    }
    double* raw() { return data.data(); }
    const double* raw() const { return data.data(); }

    std::vector<double>& asVector() { return data; }
    const std::vector<double>& asVector() const { return data; }
};

struct Array2D {
    std::vector<double> data;
    size_t nx, ny;

    Array2D(size_t nx_, size_t ny_) : data(nx_ * ny_), nx(nx_), ny(ny_) {}

    inline double& operator()(size_t i, size_t j) {
        return data[i * ny + j];
    }
    inline const double& operator()(size_t i, size_t j) const {
        return data[i * ny + j];
    }

    double* raw() { return data.data(); }
    const double* raw() const { return data.data(); }

    std::vector<double>& asVector() { return data; }
    const std::vector<double>& asVector() const { return data; }
};

struct Array2Dint {
    std::vector<int> data;
    size_t nx, ny;

    Array2Dint(size_t nx_, size_t ny_) : data(nx_ * ny_), nx(nx_), ny(ny_) {}

    inline int& operator()(size_t i, size_t j) {
        return data[i * ny + j];
    }
    inline const int& operator()(size_t i, size_t j) const {
        return data[i * ny + j];
    }

    int* raw() { return data.data(); }
    const int* raw() const { return data.data(); }

    std::vector<int>& asVector() { return data; }
    const std::vector<int>& asVector() const { return data; }
};

struct Array3D {
    std::vector<double> data;
    size_t nx, ny, nz;
    Array3D(size_t nx_, size_t ny_, size_t nz_)
        : data(nx_ * ny_ * nz_), nx(nx_), ny(ny_), nz(nz_) {}

    inline double& operator()(size_t i, size_t j, size_t k) {
        return data[(i * ny + j) * nz + k];
    }
    inline const double& operator()(size_t i, size_t j, size_t k) const {
        return data[(i * ny + j) * nz + k];
    }

    double* raw() { return data.data(); }
    const double* raw() const { return data.data(); }

    // For HighFive
    std::vector<double>& asVector() { return data; }
    const std::vector<double>& asVector() const { return data; }
};

struct Array3Dint {
    std::vector<int> data;
    size_t nx, ny, nz;
    Array3Dint(size_t nx_, size_t ny_, size_t nz_)
        : data(nx_ * ny_ * nz_), nx(nx_), ny(ny_), nz(nz_) {}

    inline int& operator()(size_t i, size_t j, size_t k) {
        return data[(i * ny + j) * nz + k];
    }
    inline const int& operator()(size_t i, size_t j, size_t k) const {
        return data[(i * ny + j) * nz + k];
    }

    int* raw() { return data.data(); }
    const int* raw() const { return data.data(); }

    std::vector<int>& asVector() { return data; }
    const std::vector<int>& asVector() const { return data; }
};

struct MSI_RGR_Data {
    Array3D radiance;
    Array2D longitude;
    Array2D latitude;
    Array2D mu0;
    Array2D phi0;
    Array2Dint surface_type;

    MSI_RGR_Data()
    : radiance(0, 0, 0),
      longitude(0, 0),
      latitude(0, 0),
      mu0(0, 0),
      phi0(0, 0),
      surface_type(0, 0) {}
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
