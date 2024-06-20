#ifndef LINE_FORMATER
#define LINE_FORMATER

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <algorithm>
#include <Windows.h>
#include <iomanip>
#include <locale>
#include <codecvt>

#include <commdlg.h>
#include <shlwapi.h>

// ===== Include OpenXLSX ===== //
#include <OpenXLSX.hpp>

#include "help_functions.h"
#include "csv_reader.h"

using namespace OpenXLSX;

class LineFormater {
private: 
    std::map<std::string, CSVReader*> _worksheets;
    int _count;

    double EvaluateExpression(const std::string& expression, const std::vector<std::string>& data) {
        std::istringstream tokens(expression);
        return EvaluateAddSub(tokens, data);
    }

    double EvaluateAddSub(std::istringstream& tokens, const std::vector<std::string>& data) {
        double value = EvaluateMulDiv(tokens, data);
        while (tokens) {
            char op = tokens.peek();
            if (op == '+' || op == '-') {
                tokens.get();
                double rhs = EvaluateMulDiv(tokens, data);
                if (op == '+') value += rhs;
                else value -= rhs;
            } else break;
        }
        return value;
    }

    double EvaluateMulDiv(std::istringstream& tokens, const std::vector<std::string>& data) {
        double value = EvaluateValue(tokens, data);
        while (tokens) {
            char op = tokens.peek();
            if (op == '*' || op == '/') {
                tokens.get();
                double rhs = EvaluateValue(tokens, data);
                if (op == '*') value *= rhs;
                else value /= rhs;
            } else break;
        }
        return value;
    }

    double EvaluateValue(std::istringstream& tokens, const std::vector<std::string>& data) {
        double value;
        if (isdigit(tokens.peek())) tokens >> value;
        else if (tokens.peek() == '(') {
            tokens.get(); // '('
            value = EvaluateAddSub(tokens, data);
            tokens.get(); // ')'
        } else {
            std::string token;
            while (isalnum(tokens.peek()) || tokens.peek() == '#') token += tokens.get();

            std::string valueStr = GetValueOrOriginal(token, data);
            value = std::stod(valueStr);
        }
        return value;
    }
    
    std::string GetValueOrOriginal(const std::string& input, std::vector<std::string> data) {
        size_t formatPos = input.find("#");
        if (formatPos != std::string::npos) {
            std::string key = input.substr(0, formatPos);
            std::string format = input.substr(formatPos + 1);

            // Perform transformations based on format
            if (format == "date") {
                // Date transformation
                XLDateTime date(stof(data[stoi(key)]));
                auto tmo = date.tm();

                std::ostringstream oss;
                oss << std::setw(4) << std::setfill('0') << 1900 + tmo.tm_year;
                oss << std::setw(2) << std::setfill('0') << tmo.tm_mon;
                oss << std::setw(2) << std::setfill('0') << tmo.tm_mday;

                return oss.str();
            } else if (format.substr(0, 1) == "[") {
                std::vector<std::string> parts = split(format.substr(1, format.size() - 2), ',');

                std::string sheet = parts[0];

                return std::to_string(_worksheets[sheet]->getValue(data[stoi(key)]));
            } else if (format == "count") {
                return std::to_string(_count);
            } else {
                // Handle other transformations like integer conversion, etc.
                // Add your own transformation logic here
                try {
                    return data[stoi(key)];
                } catch (const std::exception&) {
                    return input;
                }
            }
        }

        return input;
    }

    std::string ProcessNestedString_i(const std::string& input, std::vector<std::string> data) {
        size_t openPos = input.find("{");
        bool if_statement = input.substr(0, 2) == "if";
        bool math_expression = input.substr(0, 5) == "math#";
        
        if (openPos == std::string::npos) {
            size_t closePos = input.find("}");
            std::string res = GetValueOrOriginal(input.substr(0, closePos), data);
            return res + (closePos != std::string::npos? input.substr(closePos + 1) : "");
        }

        size_t closePos = openPos + 1;
        int openBracesCount = 1;

        // Find the matching closing braces
        while (openBracesCount > 0) {
            size_t nextOpenPos = input.find("{", closePos);
            size_t nextClosePos = input.find("}", closePos);

            if (nextClosePos == std::string::npos) throw std::runtime_error("Mismatched { and }");

            if (nextOpenPos != std::string::npos && nextOpenPos < nextClosePos) {
                openBracesCount++;
                closePos = nextOpenPos + 1;
            } else {
                openBracesCount--;
                closePos = nextClosePos + 1;
            }
        }

        size_t new_pos = 0;
        std::string extracted = ProcessNestedString_i(input.substr(openPos + 1, closePos - openPos - 2), data);
        size_t newStartPos = closePos;
        
        // Replace the processed part with the extracted string and continue processing
        std::string result = input.substr(0, openPos) + extracted + input.substr(newStartPos);
        newStartPos = openPos + extracted.length();

        size_t p = 0;
        std::string processed = result.substr(0, newStartPos) + ProcessNestedString_i(result.substr(newStartPos), data);

        if (if_statement) {
            size_t startCondition = 3;
            size_t endCondition = processed.find("?");
            size_t colonPos = processed.find(":");

            std::string condition   = processed.substr(startCondition, endCondition - startCondition);
            std::string trueBranch  = processed.substr(endCondition + 1, colonPos - endCondition - 1);
            std::string falseBranch = processed.substr(colonPos + 1);

            return EvaluateCondition(condition) ? trueBranch : falseBranch;
        } else if (math_expression) {
            size_t expressioPos = processed.find("#");
            size_t formatPos = processed.find("#", expressioPos + 1);
            std::string expression = processed.substr(expressioPos + 1, formatPos - expressioPos - 1);
            
            try {
                int precision = stoi(processed.substr(formatPos + 1));
                double result = EvaluateExpression(expression, data);

                std::ostringstream oss;
                oss << std::fixed << std::setprecision(precision) << result;
                return oss.str();

                return std::to_string(result);
            } catch (const std::exception&) {
                return processed;
            }
        }

        return processed;
    }

public:
    LineFormater() {}

    void SetWorksheets(std::map<std::string, CSVReader*> worksheets) {
        _worksheets = worksheets;
    }

    std::string FormatString(const std::string& input, std::vector<std::string> data, int count = 0) {
        _count = count;
        return ProcessNestedString_i(input, data);
    }

    bool EvaluateCondition(const std::string& condition) {
        size_t pos = condition.find_first_of("!=<>");
        if (pos != std::string::npos) {
            std::string op;
            if (condition.substr(pos + 1, 1) == "=") op = condition.substr(pos, 2);
            else op = condition.substr(pos, 1);
            
            std::string lhs = condition.substr(0, pos);
            std::string rhs = condition.substr(pos + op.length());

            try {
                // Handle numeric comparison
                float leftValue = stof(lhs);
                float rightValue = stof(rhs);

                if (op == "==") return leftValue == rightValue;
                else if (op == "!=") return leftValue != rightValue;
                else if (op == "<") return leftValue < rightValue;
                else if (op == ">") return leftValue > rightValue;
                else if (op == "<=") return leftValue <= rightValue;
                else if (op == ">=") return leftValue >= rightValue;
            } catch (const std::invalid_argument& e) {
                // Handle non-numeric comparisons
                if (op == "==") return lhs == rhs;
                else if (op == "!=") return lhs != rhs;
            }
        }
        
        return false; // Default to false for unknown conditions
    }
    
};

#endif