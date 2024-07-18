# Directories
BUILD_DIR  = build
OUT_DIR    = build\output
DEPLOY_DIR = deploy
CONF_DIR   = config

# Other files
BASE_FILES = $(wildcard $(OUT_DIR)/* *.csv *.json)
CONF_FILES = $(wildcard $(CONF_DIR)/*.txt)

# Executable name
EXECUTE = ReadExcel.exe
LIBRARY = libOpenXLSX.a

.PHONY: all build run create_deploy

all: build_run

build: 
	cmake --build $(BUILD_DIR)

run:
	./$(OUT_DIR)/$(EXECUTE)

build_run: build run

create_deploy:
	.\deploy.bat
