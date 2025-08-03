#include <iostream>
#include <memory>
#include "io/MSI_RGR_Reader.hpp"
#include "io/AC_CLP_Reader.hpp"
#include "io/HDF5Writer.hpp"
#include "process/CloudConstructor.hpp"

int main(int argc, char** argv) {
    try {
        // Check command line arguments //
        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] 
            << " <input_file> <output_file> <cloud_size>" << std::endl;
            return 1;
        }

        std::string msi_filepath     = argv[1];
        std::string acclp_filepath   = argv[2];
        std::string output_filepath  = argv[3];

        std::cout << "[main] Starting cloud construction processing" << std::endl;

        // Read input file //
        std::cout << "[main] Reading MSI data from: " << msi_filepath << std::endl;
        std::unique_ptr<MSI_RGR_Data> msi_data = MSI_Reader::read(msi_filepath);

        std::cout << "[main] Reading AC_CLP data from: " << acclp_filepath << std::endl;
        std::unique_ptr<AC_CLP_Data> acclp_data = AC_CLP_Reader::read(acclp_filepath);

        // Construct cloud field //
        size_t k_candidates = 100;
        size_t max_idx_distance = 400;
        size_t num_vartical_levels = acclp_data->height[0].size();
        size_t num_variables = 8;

        std::cout << "[main] Initializing CloudConstructor" << std::endl;
        CloudConstructor constructor(msi_data.get(), acclp_data.get(),
                                     k_candidates, max_idx_distance, num_vartical_levels, num_variables);

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
            "height"
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

        size_t i_min = 3500, i_max = 5500;
        size_t j_min = 0, j_max = msi_data->longitude[0].size() - 1;
        size_t H_out = i_max - i_min + 1;
        size_t W_out = j_max - j_min + 1;

        for (size_t l = 0; l < L; ++l) {
            std::vector<double> single_variable(H_out * W_out * K);
            size_t offset = l;

            for (size_t i = 0; i < H_out; ++i) {
                for (size_t j = 0; j < W_out; ++j) {
                    for (size_t k = 0; k < K; ++k) {
                        size_t src_idx = (((i + i_min) * W + (j + j_min)) * K + k) * L + offset;
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
        for (size_t idx = 0; idx < H_out * W_out; ++idx) {
            size_t h = idx / W_out;
            size_t w = idx % W_out;
            size_t src_h = h + i_min;
            size_t src_w = w + j_min;
            latitude_variable[idx] = msi_data->latitude[src_h][src_w];
            longitude_variable[idx] = msi_data->longitude[src_h][src_w];
        }

        writer.writeDataset("latitude", latitude_variable, {H_out, W_out});
        writer.writeDataset("longitude", longitude_variable, {H_out, W_out});

        // for (size_t l = 0; l < L; ++l) {
        //     std::vector<double> single_variable(H * W * K);
        //     size_t offset = l;

        //     for (size_t idx = 0; idx < H * W * K; ++idx) {
        //         single_variable[idx] = mapped_data[idx * L + offset];
        //     }
        //     writer.writeDataset(variable_names[l],
        //                         single_variable,
        //                         {H, W, K});
        // }
        
        // std::vector<double> latitude_variable(H * W);
        // std::vector<double> longitude_variable(H * W);
        // for (size_t idx = 0; idx < H * W; ++idx) {
        //     size_t h = idx / W;
        //     size_t w = idx % W;
        //     latitude_variable[idx] = msi_data->latitude[h][w];
        //     longitude_variable[idx] = msi_data->longitude[h][w];
        // }
        // writer.writeDataset("latitude", latitude_variable, {H, W});
        // writer.writeDataset("longitude", longitude_variable, {H, W});

        std::cout << "[main] Cloud construction completed successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[main] Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
