#include "DonorSelector.hpp"
#include "KDTreeSearcher.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

// MSI index -> AC_CLP index
size_t DonorSelector::findNearestACCLPindex(const std::pair<size_t, size_t>& msi_index) const {
    // KDTree search for nearest AC_CLP index
    KDTreeSearcherCoord::Point query = {
        msi_->longitude(msi_index.first, msi_index.second),
        msi_->latitude(msi_index.first, msi_index.second)
    };

    auto [nearest_index, distance] = AC_CoordKDTree_.findNearest(query);
    return nearest_index;
}

// AC_CLP index -> MSI index
std::pair<size_t, size_t> DonorSelector::findNearestMSIindex(const size_t acclp_index) const {
    // KDTree search for nearest MSI index
    KDTreeSearcherCoord::Point query = {
        acclp_->longitude[acclp_index],
        acclp_->latitude[acclp_index]
    };

    auto [nearest_index, distance] = MSI_CoordKDTree_.findNearest(query);

    size_t W = msi_->longitude.ny;
    size_t i = nearest_index / W;
    size_t j = nearest_index % W;
    return {i, j};
}

std::optional<std::pair<size_t, double>> DonorSelector::findBestDonor(std::pair<size_t, size_t> target_index) const {

    size_t num_band = msi_->radiance.nx;
    KDTreeSearcherBand::Spectrum log_query;
    for (size_t i = 0; i < num_band; ++i) {
        log_query[i] = std::log(msi_->radiance(i, target_index.first, target_index.second));
    }

    // Find the k nearest candidates in the spectral KDTree (AC_CLP index)
    auto candidate_indices = AC_SpectralKDTree_.findKNearest(log_query, k_candidates_);
    size_t closest_acclp_index = findNearestACCLPindex(target_index);

    // Find the index that satisfies the conditions
    double mu0_ij = msi_->mu0(target_index.first, target_index.second);
    double phi0_ij = msi_->phi0(target_index.first, target_index.second);
    double surface_type_ij = msi_->surface_type(target_index.first, target_index.second);

    size_t best_index = candidate_indices[0].first;
    double best_distance = candidate_indices[0].second;
    bool found = false;

    for (size_t k = 0; k < candidate_indices.size(); ++k) {
        size_t candidate_index = candidate_indices[k].first;
        auto msi_index = findNearestMSIindex(candidate_index);
        double distance = candidate_indices[k].second;
        size_t idx_diff = (msi_index.first > target_index.first) ? 
                          msi_index.first - target_index.first : 
                          target_index.first - msi_index.first;

        // Check conditions
        if (idx_diff <= max_idx_distance_ &&
            std::abs(msi_->mu0(msi_index.first, msi_index.second) - mu0_ij) < delta_mu0_ &&
            std::abs(msi_->phi0(msi_index.first, msi_index.second) - phi0_ij) < delta_phi0_ &&
            msi_->surface_type(msi_index.first, msi_index.second) == surface_type_ij) {
            best_index = candidate_index;
            best_distance = distance;
            found = true;
            break;
        }
    }

    if (!found) {
        best_index = closest_acclp_index;
        best_distance = 0.0;
    }
    return {{best_index, best_distance}};
}
