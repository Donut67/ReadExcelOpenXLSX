# Output directory
BUILD_DIR = build
OUT_DIR   = build/output

# Executable name
EXECUTE = ReadExcel.exe

.PHONY: all build run

all: build_run

build: 
	cmake --build $(BUILD_DIR)

run:
	./$(OUT_DIR)/$(EXECUTE)

build_run: build run
