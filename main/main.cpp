#include <iostream>
#include <memory>
#include "MSI_RGR_Reader.hpp"
#include "AC_CLP_Reader.hpp"
#include "AUX__2D_Reader.hpp"
#include "HDF5Writer.hpp"
#include "CloudConstructor.hpp"

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <MSI_RGR_File> <AC_CLP_File> <AUX_2D_File> <Output_HDF5_File>" << std::endl;
        return 1;
    }

    try {
        std::string msi_filepath     = argv[1];
        std::string acclp_filepath   = argv[2];
        std::string aux2d_filepath   = argv[3];
        std::string output_filepath  = argv[4];

        std::cout << "[main] Starting cloud construction processing" << std::endl;

        // Read input file //
        std::cout << "[main] Reading MSI data from: " << msi_filepath << std::endl;
        std::unique_ptr<MSI_RGR_Data> msi_data = MSI_Reader::read(msi_filepath);
        std::cout << "[main] Reading AC_CLP data from: " << acclp_filepath << std::endl;
        std::unique_ptr<AC_CLP_Data> acclp_data = AC_CLP_Reader::read(acclp_filepath);
        std::cout << "[main] Reading AUX_2D data from: " << aux2d_filepath << std::endl;
        std::unique_ptr<AUX__2D_Data> aux2d_data = AUX__2D_Reader::read(aux2d_filepath);

        // Construct cloud field //
        size_t k_candidates = 100;
        size_t max_idx_distance = 2000;
        size_t num_vartical_levels = acclp_data->height[0].size();
        size_t num_variables = 13;

        size_t DIFF_IDX = 100; // AUX_IDX - ACCLP_IDX at the same point
        size_t i_min = 2500, i_max = 4499;
        // size_t i_min = 2500, i_max = 3499;
        size_t j_min = 0, j_max = msi_data->longitude[0].size() - 1;
        size_t H_out = i_max - i_min + 1;
        size_t W_out = j_max - j_min + 1;

        std::cout << "[main] Initializing CloudConstructor" << std::endl;
        CloudConstructor constructor(msi_data.get(), acclp_data.get(), aux2d_data.get(),
                                     k_candidates, max_idx_distance, num_vartical_levels, num_variables,
                                     i_min, i_max, j_min, j_max);

        std::cout << "[main] Constructing cloud field" << std::endl;
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

        std::cout << "[main] Writing output to: " << output_filepath << std::endl;
        HDF5_Writer writer(output_filepath);

        writer.writeDataset("mapped_indices",
                            constructor.getMappedIndices(),
                            {constructor.height(), constructor.width()});
        std::cout << "[main] Writing mapped indices completed" << std::endl;
        std::cout << "[main] Writing mapped data" << std::endl;

        const auto& mapped_data = constructor.getMappedData();
        size_t H = constructor.height();
        size_t W = constructor.width();
        size_t K = constructor.verticalLevels();
        size_t L = constructor.numVariables();

        for (size_t l = 0; l < L; ++l) {
            std::vector<double> single_variable(H_out * W_out * K);
            size_t offset = l;

            for (size_t i = 0; i < H_out; ++i) {
                for (size_t j = 0; j < W_out; ++j) {
                    for (size_t k = 0; k < K; ++k) {
                        // size_t src_idx = (((i + i_min) * W + (j + j_min)) * K + k) * L + offset;
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
            size_t src_h = h + i_min;
            size_t src_w = w + j_min;
            size_t ac_idx   = ac_mapped_indices[h * W_out + w];
            size_t aux_idx  = ac_idx + DIFF_IDX;
            if (aux_idx >= aux2d_data->surfacePressure.size()) {
                throw std::out_of_range("AUX index out of range");
            }

            latitude_variable[idx]  = msi_data->latitude[src_h][src_w];
            longitude_variable[idx] = msi_data->longitude[src_h][src_w];

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
        std::cout << "[main:debug] Writing auxiliary data completed" << std::endl;

        writer.writeDataset("latitude", latitude_variable, {H_out, W_out});
        writer.writeDataset("longitude", longitude_variable, {H_out, W_out});
        writer.writeDataset("surfacePressure", surfacePressure_variable, {H_out, W_out});
        writer.writeDataset("totalColumnOzone", totalColumnOzone_variable, {H_out, W_out});
        writer.writeDataset("totalColumnWaterVapor", totalColumnWaterVapor_variable, {H_out, W_out});
        writer.writeDataset("day_night_flag", day_night_flag_variable, {H_out, W_out});
        writer.writeDataset("land_water_flag", land_water_flag_variable, {H_out, W_out});

        std::cout << "[main] Cloud construction completed successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[main] Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
