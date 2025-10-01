#pragma once
#include <memory>
#include <string>
#include "ObservationDataset.hpp"

class MSI_RGR_Reader {
public:
    static std::unique_ptr<MSI_RGR_Data> read(const std::string& filepath);
};
