#include <iostream>
#include <memory>
#include "MSI_RGR_Reader.hpp"
#include "AC_CLP_Reader.hpp"
#include "AUX__2D_Reader.hpp"
#include "HDF5Writer.hpp"
#include "CloudConstructor.hpp"

class BarkerPixelMatching {
public:
    BarkerPixelMatching(const std::string& msi_rgr_path,
                        const std::string& ac_clp_path,
                        const std::string& aux_2d_path,
                        const std::string& output_path,
                        const size_t idx_min,
                        const size_t idx_max)
        : msi_rgr_path_(msi_rgr_path),
          ac_clp_path_(ac_clp_path),
          aux_2d_path_(aux_2d_path),
          output_path_(output_path),
          idx_min_(idx_min),
          idx_max_(idx_max) {}

    static void run(const std::string& msi_rgr_path,
                    const std::string& ac_clp_path,
                    const std::string& aux_2d_path,
                    const std::string& output_path,
                    const size_t idx_min,
                    const size_t idx_max) {
        BarkerPixelMatching bpm(msi_rgr_path, 
                               ac_clp_path, 
                               aux_2d_path, 
                               output_path, 
                               idx_min, idx_max);
        bpm.execute();
        return;
    }

private:
    void execute();

    const std::string msi_rgr_path_;
    const std::string ac_clp_path_;
    const std::string aux_2d_path_;
    const std::string output_path_;
    size_t idx_min_;
    size_t idx_max_;
};
