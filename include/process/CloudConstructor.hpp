#pragma once

#include <vector>
#include <utility>
#include "io/ObservationDataset.hpp"
#include "process/DonorSelector.hpp"
#include "process/KDTreeSearcher.hpp"

class CloudConstructor {
public:
    using Index2D = std::pair<size_t, size_t>;
    using Index3D = std::tuple<size_t, size_t, size_t>;

    CloudConstructor(const MSI_RGR_Data* msi, 
                     AC_CLP_Data* acclp,
                     size_t k_candidates = 100,
                     size_t max_idx_distance = 400, 
                     size_t num_vertical_levels = 0,
                     size_t num_variables = 0);

    // Processing function
    void construct();

    // Accessors for results
    const std::vector<size_t>& getMappedIndices() const { return mapped_indices_; }
    const std::vector<double>& getMappedData() const { return mapped_data_; }

    size_t height() const { return H_; }
    size_t width() const { return W_; }
    size_t verticalLevels() const { return K_; }
    size_t numVariables() const { return L_; }

    inline size_t flatIndex(size_t i, size_t j, size_t k, size_t l) const {
        return (((i * W_ + j) * K_ + k) * L_) + l;
    }
                     
private:
    void mapVariables(size_t i, size_t j, size_t ac_idx);

    const MSI_RGR_Data* msi_;
    AC_CLP_Data* acclp_;

    size_t k_candidates_;
    size_t max_idx_distance_;
    
    // KDTrees
    KDTreeSearcherBand  AC_LogSpectralKDTree_;
    KDTreeSearcherCoord AC_CoordKDTree_;
    KDTreeSearcherCoord MSI_CoordKDTree_;

    // DonorSelector
    DonorSelector donor_selector_;

    // Dimensions
    size_t H_;  // Height of the MSI data
    size_t W_;  // Width of the MSI data
    size_t K_;  // Number of vertical levels
    size_t L_;  // Number of variables

    // Results
    std::vector<size_t> mapped_indices_;  // mapped indices (i,j) -> (k,l)
    std::vector<double> mapped_data_;
};
