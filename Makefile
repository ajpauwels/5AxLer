# Compiler
CC = g++

# Default flags
CFLAGS = -g -Wall -std=c++11

# Target executable and program entry point
TARGET = 5AxLer
ENTRY = main.cpp

# Various directories
SRC_DIR = ./src/
BUILD_DIR = ./build/
LIB_DIR = ./libs/
TEST_DIR = ./tests/

# make default call
all: $(TARGET)

# To make the final program
$(TARGET): $(SRC_DIR)$(ENTRY) Mesh.o Vector3D.o Angle.o ProcessSTL.o Clock.o Plane.o Clipper.o Slicer.o Slice.o Island.o Polygon.o
	$(CC) $(CFLAGS) $(BUILD_DIR)Island.o $(BUILD_DIR)Polygon.o $(BUILD_DIR)Slicer.o $(BUILD_DIR)Slice.o $(BUILD_DIR)Clipper.o $(BUILD_DIR)Plane.o $(BUILD_DIR)Mesh.o $(BUILD_DIR)Vector3D.o $(BUILD_DIR)Angle.o $(BUILD_DIR)ProcessSTL.o $(BUILD_DIR)Clock.o -o $(BUILD_DIR)$(TARGET) $(SRC_DIR)$(ENTRY)

# Build the Mesh object file
Mesh.o: $(SRC_DIR)Mesh.cpp $(SRC_DIR)Mesh.hpp $(SRC_DIR)Utility.hpp $(SRC_DIR)Vector3D.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Mesh.o $(SRC_DIR)Mesh.cpp

# Build the Vector3D object file
Vector3D.o: $(SRC_DIR)Vector3D.cpp $(SRC_DIR)Vector3D.hpp $(SRC_DIR)Utility.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Vector3D.o $(SRC_DIR)Vector3D.cpp

# Build the Angle object file
Angle.o: $(SRC_DIR)Angle.cpp $(SRC_DIR)Angle.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Angle.o $(SRC_DIR)Angle.cpp

# Build the Clock object file
Clock.o: $(SRC_DIR)Clock.cpp $(SRC_DIR)Clock.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Clock.o $(SRC_DIR)Clock.cpp

# Build the BuildMap object file
BuildMap.o: $(SRC_DIR)BuildMap.cpp $(SRC_DIR)BuildMap.hpp $(SRC_DIR)Utility.hpp $(SRC_DIR)Vector3D.hpp $(SRC_DIR)Angle.hpp $(LIB_DIR)clipper/clipper.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)BuildMap.o $(SRC_DIR)BuildMap.cpp

# Build the ProcessSTL object file
ProcessSTL.o: $(SRC_DIR)ProcessSTL.cpp $(SRC_DIR)ProcessSTL.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)ProcessSTL.o $(SRC_DIR)ProcessSTL.cpp

# Build the VolumeDecomposer object file
VolumeDecomposer.o: $(SRC_DIR)VolumeDecomposer.cpp $(SRC_DIR)VolumeDecomposer.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)VolumeDecomposer.o $(SRC_DIR)VolumeDecomposer.cpp

# Build the Island object file
Island.o: $(SRC_DIR)Island.cpp $(SRC_DIR)Island.hpp $(SRC_DIR)Utility.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Island.o $(SRC_DIR)Island.cpp

# Build the Plane object file
Plane.o: $(SRC_DIR)Plane.cpp $(SRC_DIR)Plane.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Plane.o $(SRC_DIR)Plane.cpp

# Make the Polygon object file
Polygon.o: $(SRC_DIR)Polygon.cpp $(SRC_DIR)Polygon.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Polygon.o $(SRC_DIR)Polygon.cpp

# Make the Slice object file
Slice.o: $(SRC_DIR)Slice.cpp $(SRC_DIR)Slice.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Slice.o $(SRC_DIR)Slice.cpp

# Make the Slicer object file
Slicer.o: $(SRC_DIR)Slicer.cpp $(SRC_DIR)Slicer.hpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Slicer.o $(SRC_DIR)Slicer.cpp

# Make the clipper object file
Clipper.o: $(LIB_DIR)clipper/clipper.cpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Clipper.o $(LIB_DIR)clipper/clipper.cpp

# Build the Tests object file
# Tests.o: $(TEST_DIR)
# 	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Tests.o $(TEST_DIR)Tests.cpp

# To clean the program
clean:
	$(RM) $(BUILD_DIR)*