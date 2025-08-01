# baker-pixel-matching
C++ implementation of the pixel matching based cloud construction algorithm in Barker et al. (2011) to process EarthCARE satellite data.

### Input Data
- MSI_RGR: Multi-Spectral Imager L1C products
- AC_CLP: Cloud Profiling Radar and Atmospheric Lidar L2 products

### Usage
`./bin/cloud_construct <MSI_RGR_FILE> <AC_CLP_FILE> <OUTPUT_FILE>`

### Requirements
- C++ compiler (C++11 or later)
- hdf5 library

### Third-Party Libraries

This project uses the following open-source libraries:

- [nanoflann](https://github.com/jlblancoc/nanoflann): A C++ header-only library for nearest neighbor search in metric spaces.
  Licence: BSD-2-Clause
  
- [HighFive](https://github.com/BlueBrain/HighFive): A C++ header-only library for HDF5 file handling.
  Licence: Boost Software License 1.0

These libraries are included in `/external`.

### Reference
Barker, H. W., et al. "A 3D cloud-construction algorithm for the EarthCARE satellite mission." Quarterly Journal of the Royal Meteorological Society 137.657 (2011): 1042-1058.
