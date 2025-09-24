#pragma once
#include <string>
#include <vector>
#include <H5Cpp.h>

class HDF5_Writer {
public:
    explicit HDF5_Writer(const std::string& filepath);

    // size_t type dataset
    void writeDataset(const std::string& name,
                      const std::vector<size_t>& data,
                      const std::vector<size_t>& shape);

    // double type dataset
    void writeDataset(const std::string& name,
                      const std::vector<double>& data,
                      const std::vector<size_t>& shape);

    // int type dataset
    void writeDataset(const std::string& name,
                      const std::vector<int>& data,
                      const std::vector<size_t>& shape);

private:
    H5::H5File file_;
};
