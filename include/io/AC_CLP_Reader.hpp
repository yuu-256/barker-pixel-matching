#pragma once
#include <memory>
#include <string>
#include "io/ObservationDataset.hpp"

class AC_CLP_Reader {
public:
    static std::unique_ptr<AC_CLP_Data> read(const std::string& filepath);
};
