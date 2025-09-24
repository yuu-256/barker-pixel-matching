#pragma once
#include <memory>
#include <string>
#include "ObservationDataset.hpp"

class AUX__2D_Reader {
public:
    static std::unique_ptr<AUX__2D_Data> read(const std::string& filepath);
};
