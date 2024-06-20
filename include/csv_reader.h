#ifndef CSV_READER
#define CSV_READER

#include <unordered_map>
#include <fstream>

// ===== Include OpenXLSX ===== //
#include <OpenXLSX.hpp>

#include "help_functions.h"

using namespace OpenXLSX;

class CSVReader {
private:
    std::unordered_map<std::string, long long int> _data;
    long long int _initial;
    std::string _filename, _header;

    std::string generate_value(const std::string& name) {
        if (_data.find(name) == _data.end()) {
            long long int newValue = _initial + _data.size();
            _data[name] = newValue;
            return std::to_string(newValue);
        } else return std::to_string(_data[name]);
    }

public:
    CSVReader() {
        _initial = 4300000001LL;
    }
    CSVReader(const std::string& filename) {
        _initial = 4300000001LL;
        _filename = filename;

        std::ifstream file(_filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << _filename << std::endl;
            return;
        }

        // Skip the header line
        std::string line;
        std::getline(file, _header);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string numStr, name;
            if (std::getline(ss, numStr, ';') && std::getline(ss, name, ';')) _data[name] = std::stoll(numStr);
        }

        file.close();
    }

    long long int getValue(std::string value) {
        return _data[value];
    }

    void updateNames(const std::string& filename, const std::string& fieldName) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Read the header line
        std::string header;
        std::getline(file, header);
        std::vector<std::string> fieldNames = split(header, ';');

        // Find the index of the field to check for update
        size_t fieldIndex = 0;
        for (size_t i = 0; i < fieldNames.size(); ++i) {
            if (fieldNames[i] == fieldName) {
                fieldIndex = i;
                break;
            }
        }

        // Read data lines
        std::string line;
        while (std::getline(file, line)) generate_value(split(line, ';')[fieldIndex]);

        file.close();
        saveFile();
    }

    void updateNamesXLSX(const std::string& filename, const std::string& fieldName) {
        XLDocument doc;

        doc.open(filename);
        auto wks = doc.workbook().worksheet("A");
        size_t fieldIndex = -1;

        for(auto row : wks.rows()) {
            if(std::vector<XLCellValue>(row.values())[0].type() == XLValueType::Empty) break;
            else if(row.rowNumber() != 1) {
                std::string client = std::vector<XLCellValue>(row.values())[fieldIndex];
                generate_value(client);
            } else if(row.rowNumber() == 1) {
                auto fieldNames = std::vector<XLCellValue>(row.values());
                for (size_t i = 0; i < fieldNames.size(); i++) {
                    std::ostringstream oss;
                    oss << fieldNames[i];
                    if (oss.str() == fieldName) {
                        fieldIndex = i;
                        break;
                    }
                }
                if (fieldIndex == -1) throw("Incorrect Field Name [UpdateNameXLSX]");
            }
        }

        doc.close();
        saveFile();
    }

    void saveFile() const {
        if(_filename != ""){
            std::ofstream file(_filename);
            file << _header << "\n";
            for (const auto& pair : _data) {
                file << pair.second << ";" << pair.first << std::endl;
            }
        }
    }
};

#endif