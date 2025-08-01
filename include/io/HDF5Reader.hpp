#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class HDF5Reader {
public:
    explicit HDF5Reader(const std::string& filepath) : filepath_(filepath) {};
    virtual ~HDF5Reader() = default;

private:
    std::string filepath_;
};
