#include "HDF5Writer.hpp"
#include <H5Cpp.h>
#include <stdexcept>

HDF5_Writer::HDF5_Writer(const std::string& filepath)
    : file_(filepath, H5F_ACC_TRUNC) {}

void HDF5_Writer::writeDataset(const std::string& name,
                               const std::vector<size_t>& data,
                               const std::vector<size_t>& shape) {
    H5::DataSpace dataspace(shape.size(), reinterpret_cast<const hsize_t*>(shape.data()));
    H5::DataSet dataset = file_.createDataSet(name, H5::PredType::NATIVE_ULLONG, dataspace);
    dataset.write(data.data(), H5::PredType::NATIVE_ULLONG);
}

void HDF5_Writer::writeDataset(const std::string& name,
                               const std::vector<double>& data,
                               const std::vector<size_t>& shape) {
    H5::DataSpace dataspace(shape.size(), reinterpret_cast<const hsize_t*>(shape.data()));
    H5::DataSet dataset = file_.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(data.data(), H5::PredType::NATIVE_DOUBLE);
}

void HDF5_Writer::writeDataset(const std::string& name,
                               const std::vector<int>& data,
                               const std::vector<size_t>& shape) {
    H5::DataSpace dataspace(shape.size(), reinterpret_cast<const hsize_t*>(shape.data()));
    H5::DataSet dataset = file_.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(data.data(), H5::PredType::NATIVE_DOUBLE);
}
