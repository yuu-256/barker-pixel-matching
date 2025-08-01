CXX			 := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude -Iexternal -Iexternal/HighFive/include

HDF5_FLAGS := $(shell pkg-config --cflags hdf5)
HDF5_LIBS	 := $(shell pkg-config --libs hdf5) -lhdf5_cpp

SRC_DIR		 := src
INC_DIR		 := include
MAIN_DIR	 := main
BUILD_DIR	 := build
BIN_DIR		 := bin

TARGET := $(BIN_DIR)/cloud_constructor

# Source files
SRC_FILES := \
    $(SRC_DIR)/process/CloudConstructor.cpp \
    $(SRC_DIR)/process/DonorSelector.cpp \
    $(SRC_DIR)/io/AC_CLP_Reader.cpp \
    $(SRC_DIR)/io/HDF5Writer.cpp \
    $(SRC_DIR)/io/MSI_RGR_Reader.cpp \
    $(MAIN_DIR)/main.cpp

OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@ $(HDF5_LIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(HDF5_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(TARGET)
	./$(TARGET) input_msi.h5 input_acclp.h5 output.h5
