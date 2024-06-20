#ifndef PARSE_CONFIGURATIONS
#define PARSE_CONFIGURATIONS

#include "line_formater.h"
#include "help_functions.h"

class ParseConfigurations {
private:
    std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> _configs;
    LineFormater formater;
    int _count;

public:
    ParseConfigurations(){}
    ParseConfigurations(const std::string &configFilePath, std::map<std::string, CSVReader*> worksheets, int count = 0) {
        _count = count;

        std::ifstream configFile(configFilePath);
        if (configFile.is_open()) {
            std::string line, condition;
            std::vector<std::vector<std::string>> configs;

            while (std::getline(configFile, line)) {
                std::vector<std::string> config = split(line, ';');
                if (config.size() > 1) configs.push_back(config);
                else {
                    if (condition != "") _configs.push_back({condition, configs});
                    condition = config[0];
                    configs = {};
                }
            }
            _configs.push_back({condition, configs});
            configFile.close();
        } else std::cerr << "Error: Unable to open config file. \'" << configFilePath << "\'" << std::endl;

        formater.SetWorksheets(worksheets);
    }

    std::string ParseLine(std::string line) {
        std::string new_lines;

        std::vector<std::string> values = split(line, ';');

        for (const auto &pair : _configs) {
            std::string condition = formater.FormatString(pair.first, values);
            const std::vector<std::vector<std::string>> &configs = pair.second;

            if (formater.EvaluateCondition(condition)) {
                for (const auto &config : configs) {
                    std::string derivedLine;

                    for(const std::string& cell : config) {
                        std::string format = formater.FormatString(cell, values, _count);
                        // std::cout << format << " ";
                        derivedLine += format + ";";
                    }
                    derivedLine.pop_back();

                    new_lines += derivedLine + "\n";
                }
                break;
            }
        }

        _count++;
        return new_lines;
    }   

    int get_count() const {
        return _count;
    }

};

#endif