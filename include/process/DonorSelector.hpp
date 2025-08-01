#pragma once

#include <vector>
#include <array>
#include <optional>
#include "io/ObservationDataset.hpp"
#include "KDTreeSearcher.hpp"

class DonorSelector {
public:
    using Spectrum = std::vector<double>;
     
    DonorSelector(const MSI_RGR_Data* msi_data,
                  const AC_CLP_Data* acclp_data,
                  const std::vector<double>& weights = {},
                  size_t k_candidates = 100,
                  size_t max_idx_distance = 400,
                  const KDTreeSearcherBand& AC_LogSpectralKDTree = KDTreeSearcherBand(),
                  const KDTreeSearcherCoord& AC_CoordKDTree = KDTreeSearcherCoord(),
                  const KDTreeSearcherCoord& MSI_CoordKDTree = KDTreeSearcherCoord())
        : msi_(msi_data),
          acclp_(acclp_data),
          weights_(weights),
          k_candidates_(k_candidates),
          max_idx_distance_(max_idx_distance),
          AC_SpectralKDTree_(AC_LogSpectralKDTree),
          AC_CoordKDTree_(AC_CoordKDTree),
          MSI_CoordKDTree_(MSI_CoordKDTree) {};

    std::optional<std::pair<size_t, double>> findBestDonor(std::pair<size_t, size_t> msi_index) const;

private:
    // private member functions
    size_t findNearestACCLPindex(const std::pair<size_t, size_t>& msi_index) const;
    std::pair<size_t, size_t> findNearestMSIindex(size_t acclp_index) const;
    
    // Data members
    const MSI_RGR_Data* msi_;
    const AC_CLP_Data* acclp_;
    std::vector<double> weights_;
    size_t k_candidates_;
    size_t max_idx_distance_;
    const KDTreeSearcherBand& AC_SpectralKDTree_;
    const KDTreeSearcherCoord& AC_CoordKDTree_;
    const KDTreeSearcherCoord& MSI_CoordKDTree_;
    double delta_mu0_ = 30.0; // Default value for mu0 difference threshold
    double delta_phi0_ = 30.0; // Default value for phi0 difference threshold
};
