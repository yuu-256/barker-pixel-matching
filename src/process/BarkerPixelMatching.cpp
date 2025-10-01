#include <iostream>
#include <memory>
#include "MSI_RGR_Reader.hpp"
#include "AC_CLP_Reader.hpp"
#include "AUX__2D_Reader.hpp"
#include "HDF5Writer.hpp"
#include "CloudConstructor.hpp"
#include "BarkerPixelMatching.hpp"

void BarkerPixelMatching::execute() {

    try {
        // Read input file //
        std::unique_ptr<MSI_RGR_Data> msi_data      = MSI_RGR_Reader::read(msi_rgr_path_);
        std::unique_ptr<AC_CLP_Data> acclp_data     = AC_CLP_Reader::read(ac_clp_path_);
        std::unique_ptr<AUX__2D_Data> aux2d_data    = AUX__2D_Reader::read(aux_2d_path_);

        // Construct cloud field //
        size_t k_candidates = 100;
        size_t max_idx_distance = 2000;
        size_t num_vartical_levels = acclp_data->height[0].size();
        size_t num_variables = 13;

        size_t DIFF_IDX = 100; // AUX_IDX - ACCLP_IDX at the same point
        size_t j_min = 0, j_max = msi_data->longitude.ny - 1;
        size_t H_out = idx_max_ - idx_min_ + 1;
        size_t W_out = j_max - j_min + 1;

        CloudConstructor constructor(msi_data.get(), acclp_data.get(), aux2d_data.get(),
                                     k_candidates, max_idx_distance, num_vartical_levels, num_variables,
                                     idx_min_, idx_max_, j_min, j_max);

        constructor.construct();

        // Output to HDF5 file //
        std::vector<std::string> variable_names = {
            "cloud_effective_radius1",
            "cloud_effective_radius2",
            "cloud_water_content1",
            "cloud_water_content2",
            "cloud_phase1",
            "cloud_phase2",
            "radar_lidar_flag",
            "height",
            "ozoneMassMixingRatio",
            "pressure",
            "specificHumidity",
            "temperature",
            "height_aux"
        };

        HDF5_Writer writer(output_path_);

        writer.writeDataset("mapped_indices",
                            constructor.getMappedIndices(),
                            {constructor.height(), constructor.width()});

        const auto& mapped_data = constructor.getMappedData();
        size_t K = constructor.verticalLevels();
        size_t L = constructor.numVariables();

        for (size_t l = 0; l < L; ++l) {
            std::vector<double> single_variable(H_out * W_out * K);
            size_t offset = l;

            for (size_t i = 0; i < H_out; ++i) {
                for (size_t j = 0; j < W_out; ++j) {
                    for (size_t k = 0; k < K; ++k) {
                        // size_t src_idx = (((i + idx_min_) * W + (j + j_min)) * K + k) * L + offset;
                        size_t src_idx = ((i * W_out + j) * K + k) * L + offset;
                        size_t dst_idx = (i * W_out + j) * K + k;
                        single_variable[dst_idx] = mapped_data[src_idx];
                    }
                }
            }
            writer.writeDataset(variable_names[l],
                                single_variable,
                                {H_out, W_out, K});
        }

        std::vector<double> latitude_variable(H_out * W_out);
        std::vector<double> longitude_variable(H_out * W_out);

        std::vector<size_t> ac_mapped_indices = constructor.getMappedIndices();
        std::vector<double> surfacePressure_variable(H_out * W_out);
        std::vector<double> totalColumnOzone_variable(H_out * W_out);
        std::vector<double> totalColumnWaterVapor_variable(H_out * W_out);
        std::vector<int> day_night_flag_variable(H_out * W_out);
        std::vector<int> land_water_flag_variable(H_out * W_out);

        for (size_t idx = 0; idx < H_out * W_out; ++idx) {
            size_t h = idx / W_out;
            size_t w = idx % W_out;
            size_t src_h = h + idx_min_;
            size_t src_w = w + j_min;
            size_t ac_idx   = ac_mapped_indices[h * W_out + w];
            size_t aux_idx  = ac_idx + DIFF_IDX;
            if (aux_idx >= aux2d_data->surfacePressure.size()) {
                throw std::out_of_range("AUX index out of range");
            }

            latitude_variable[idx]  = msi_data->latitude(src_h, src_w);
            longitude_variable[idx] = msi_data->longitude(src_h, src_w);

            if (ac_idx == std::numeric_limits<size_t>::max()) {
                surfacePressure_variable[idx]       = std::numeric_limits<double>::quiet_NaN();
                totalColumnOzone_variable[idx]      = std::numeric_limits<double>::quiet_NaN();
                totalColumnWaterVapor_variable[idx] = std::numeric_limits<double>::quiet_NaN();
                day_night_flag_variable[idx]        = -1;
                land_water_flag_variable[idx]       = -1;
            } else {
                surfacePressure_variable[idx]       = aux2d_data->surfacePressure[aux_idx];
                totalColumnOzone_variable[idx]      = aux2d_data->totalColumnOzone[aux_idx];
                totalColumnWaterVapor_variable[idx] = aux2d_data->totalColumnWaterVapor[aux_idx];
                day_night_flag_variable[idx]        = aux2d_data->day_night_flag[aux_idx];
                land_water_flag_variable[idx]       = aux2d_data->land_water_flag[aux_idx];
            }
        }

        writer.writeDataset("latitude", latitude_variable, {H_out, W_out});
        writer.writeDataset("longitude", longitude_variable, {H_out, W_out});
        writer.writeDataset("surfacePressure", surfacePressure_variable, {H_out, W_out});
        writer.writeDataset("totalColumnOzone", totalColumnOzone_variable, {H_out, W_out});
        writer.writeDataset("totalColumnWaterVapor", totalColumnWaterVapor_variable, {H_out, W_out});
        writer.writeDataset("day_night_flag", day_night_flag_variable, {H_out, W_out});
        writer.writeDataset("land_water_flag", land_water_flag_variable, {H_out, W_out});

    }
    catch (const std::exception& e) {
        std::cerr << "[main] Error: " << e.what() << std::endl;
    }
}
