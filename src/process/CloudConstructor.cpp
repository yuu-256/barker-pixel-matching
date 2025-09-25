#include "CloudConstructor.hpp"
#include <iostream>
#include <cmath>

CloudConstructor::CloudConstructor(const MSI_RGR_Data* msi_data,
                                   AC_CLP_Data* acclp_data,
                                   const AUX__2D_Data* aux2d_data,
                                   size_t k_candidates,
                                   size_t max_idx_distance,
                                   size_t num_vertical_levels,
                                   size_t num_variables,
                                   size_t i_min, size_t i_max,
                                   size_t j_min, size_t j_max)
    : msi_(msi_data), 
      acclp_(acclp_data),
      aux2d_(aux2d_data),
      k_candidates_(k_candidates),
      max_idx_distance_(max_idx_distance),
      AC_LogSpectralKDTree_(),
      AC_CoordKDTree_(),
      MSI_CoordKDTree_(),
      donor_selector_(msi_data, acclp_data, {}, k_candidates, max_idx_distance,
                      AC_LogSpectralKDTree_, AC_CoordKDTree_, MSI_CoordKDTree_),
      H_(msi_data->longitude.size()),
      W_(msi_data->longitude[0].size()),
      K_(num_vertical_levels),
      L_(num_variables),
      H_out_(i_max - i_min + 1),
      W_out_(j_max - j_min + 1),
      i_min_(i_min), i_max_(i_max),
      j_min_(j_min), j_max_(j_max)
{
    std::cout << "[CloudConstructor] Using k_candidates: " << k_candidates_ << std::endl;
    std::cout << "[CloudConstructor] Using max_idx_distance: " << max_idx_distance_ << std::endl;

    // MSI Coordinate KDTree //
    std::vector<KDTreeSearcherCoord::Point> msi_coords;
    msi_coords.reserve(H_ * W_);
    for (size_t i = 0; i < H_; ++i) {
        for (size_t j = 0; j < W_; ++j) {
            msi_coords.push_back({msi_->longitude[i][j], msi_->latitude[i][j]});
        }
    }
    MSI_CoordKDTree_.setData(msi_coords);

    // Copy nearest MSI radiance data to AC //
    size_t num_ac_points = acclp_->longitude.size();
    size_t num_bands = msi_->radiance[0][0].size();
    acclp_->radiance.resize(num_ac_points);

    for (size_t i = 0; i < num_ac_points; ++i) {
        KDTreeSearcherCoord::Point query = {acclp_->longitude[i], acclp_->latitude[i]};
        auto [nearest_index, distance] = MSI_CoordKDTree_.findNearest(query);
        if (nearest_index >= H_ * W_) {
            std::cerr << "Error: Nearest index out of bounds: " << nearest_index << std::endl;
            continue;
        }

        size_t msi_i = nearest_index / W_;
        size_t msi_j = nearest_index % W_;

        KDTreeSearcherBand::Spectrum spectrum;
        for (size_t b = 0; b < num_bands; ++b) {
            spectrum[b] = std::log(msi_->radiance[msi_i][msi_j][b]);
        }
        acclp_->radiance[i] = spectrum;
    }

    // AC_CLP Spectral KDTree //
    AC_LogSpectralKDTree_.setData(acclp_->radiance);
    
    // AC_CLP Coordinate KDTree //
    std::vector<KDTreeSearcherCoord::Point> acclp_coords(num_ac_points);
    for (size_t i = 0; i < num_ac_points; ++i) {
        acclp_coords[i] = {acclp_->longitude[i], acclp_->latitude[i]};
    }
    AC_CoordKDTree_.setData(acclp_coords);

    // Initialize mapped indices and data structures
    // mapped_indices_.assign(H_ * W_, 0);
    // mapped_data_.assign(H_ * W_ * K_ * L_, std::numeric_limits<double>::quiet_NaN());
    mapped_indices_.assign(H_out_ * W_out_, 0);
    mapped_data_.assign(H_out_ * W_out_ * K_ * L_, std::numeric_limits<double>::quiet_NaN());
}

void CloudConstructor::construct() {
    std::cout << "[CloudConstructor] Starting cloud construction" << std::endl;

    // Iterate over each pixel in the MSI data
    for (size_t i = 0; i < H_out_; ++i) {
        for (size_t j = 0; j < W_out_; ++j) {
            size_t src_i = i + i_min_;
            size_t src_j = j + j_min_;
            auto result = donor_selector_.findBestDonor({src_i, src_j});

            if (!result.has_value()) {
                mapped_indices_[i * W_ + j] = std::numeric_limits<size_t>::max();
                for (size_t k = 0; k < K_; ++k) {
                    for (size_t l = 0; l < L_; ++l) {
                        size_t idx = flatIndex(i, j, k, l);
                        mapped_data_[idx] = std::numeric_limits<double>::quiet_NaN();
                    }
                }
                continue;
            }

            size_t ac_idx = result->first;
            mapped_indices_[i * W_ + j] = ac_idx;
            mapVariables(i, j, ac_idx);
        }
    }
    std::cout << "[CloudConstructor] Cloud construction completed successfully" << std::endl;
}

void CloudConstructor::mapVariables(size_t i, size_t j, size_t ac_idx) {
    size_t aux_idx = ac_idx + DEFF_IDX_;
    for (size_t k = 0; k < K_; ++k) {
        mapped_data_[flatIndex(i, j, k, 0)] = acclp_->cloud_effective_radius1[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 1)] = acclp_->cloud_effective_radius2[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 2)] = acclp_->cloud_water_content1[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 3)] = acclp_->cloud_water_content2[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 4)] = acclp_->cloud_phase1[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 5)] = acclp_->cloud_phase2[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 6)] = acclp_->radar_lidar_flag[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 7)] = acclp_->height[ac_idx][k];
        mapped_data_[flatIndex(i, j, k, 8)] = aux2d_->ozoneMassMixingRatio[aux_idx][K_ - 1 - k];
        mapped_data_[flatIndex(i, j, k, 9)] = aux2d_->pressure[aux_idx][K_ - 1 - k];
        mapped_data_[flatIndex(i, j, k, 10)] = aux2d_->specificHumidity[aux_idx][K_ - 1 - k];
        mapped_data_[flatIndex(i, j, k, 11)] = aux2d_->temperature[aux_idx][K_ - 1 - k];
        mapped_data_[flatIndex(i, j, k, 12)] = aux2d_->height[aux_idx][K_ - 1 - k];
    }
}
