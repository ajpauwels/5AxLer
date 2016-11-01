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
$(TARGET): $(SRC_DIR)$(ENTRY) Mesh.o Vector3D.o Angle.o Tests.o
	$(CC) $(CFLAGS) $(BUILD_DIR)Mesh.o $(BUILD_DIR)Vector3D.o $(BUILD_DIR)Angle.o $(BUILD_DIR)Tests.o -o $(BUILD_DIR)$(TARGET) $(SRC_DIR)$(ENTRY)

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

# Build the Tests object file
Tests.o: $(TEST_DIR)Tests.cpp
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)Tests.o $(TEST_DIR)Tests.cpp

# To clean the program
clean:
	$(RM) $(BUILD_DIR)*