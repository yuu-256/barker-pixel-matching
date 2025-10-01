#include <iostream>
#include <memory>
#include <string>
#include "MSI_RGR_Reader.hpp"
#include "ObservationDataset.hpp"
#include "BarkerPixelMatching.hpp"

std::vector<std::pair<size_t, size_t>> getDividedIndex(const Array3D& array, 
                                                       const size_t unit_size,
                                                       const size_t overlap_size) {
    size_t height   = array.ny;
    std::vector<std::pair<size_t, size_t>> index_pairs;
    
    for (size_t start = 0; start < height; start += (unit_size - overlap_size)) {
        size_t end = std::min(start + unit_size - 1, height - 1);
        index_pairs.emplace_back(start, end);
        if (end == height - 1) {
            break;
        }
    }

    return index_pairs;
}

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << "<msi_rgr_filepath> "
                                          << "<ac_clp_filepath> "
                                          << "<aux_2d_filepath> "
                                          << "<output_dirpath> "
                                          << "<unit_size> "
                                          << "<overlap_size> "
                                          << std::endl;
        return 1;
    }

    try {
        std::string msi_rgr_filepath    = argv[1];
        std::string ac_clp_filepath     = argv[2];
        std::string aux_2d_filepath     = argv[3];
        std::string output_dirpath      = argv[4];
        size_t unit_size                = std::stoul(argv[5]);
        size_t overlap_size             = std::stoul(argv[6]);

        // Read the MSI_RGR file
        auto msi_rgr_ptr = MSI_RGR_Reader::read(msi_rgr_filepath);
        MSI_RGR_Data& msi_rgr = *msi_rgr_ptr;

        // Get the data array
        std::vector<std::pair<size_t, size_t>> index_pairs =
        getDividedIndex(msi_rgr.radiance, unit_size, overlap_size);

        // Output the divided indices
        for (const auto& pair : index_pairs) {
            size_t id = pair.first / (unit_size - overlap_size);
            std::string output_filepath = output_dirpath + "/ATM_BPM_P" + std::to_string(id) + ".h5";
            BarkerPixelMatching::run(msi_rgr_filepath, 
                                     ac_clp_filepath, 
                                     aux_2d_filepath,
                                     output_filepath,
                                     pair.first, 
                                     pair.second);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
