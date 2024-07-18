#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include <string>
#include <vector>
#include <codecvt>
#include <algorithm>
#include <locale>
#include <sstream>
#include <cctype>
#include <cmath>
#include <unordered_map>
#include <stack>

std::string wctos(wchar_t *value) {
    std::wstring ws(value);
    std::string str(ws.begin(), ws.end());
    return str;
}

std::string cwctos(const wchar_t *value) {
    std::wstring ws(value);
    std::string str(ws.begin(), ws.end());
    return str;
}

std::string WStringToString(const std::wstring& wstr){
	std::string str;
	size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

std::wstring StringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::istringstream iss(s);
    std::string cellData;
    std::vector<std::string> cells;

    while (std::getline(iss, cellData, delimiter)) cells.push_back(cellData);

    return cells;
}

int Precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

double ApplyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return std::pow(a, b);
    }
    return 0;
}

bool IsOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool IsNumber(char c) {
    return std::isdigit(c) || c == '.';
}

void ProcessOperator(std::stack<double>& values, std::stack<char>& ops) {
    double val2 = values.top();
    values.pop();
    
    double val1 = values.top();
    values.pop();
    
    char op = ops.top();
    ops.pop();
    
    values.push(ApplyOp(val1, val2, op));
}

double EvaluateExpression(const std::string& expression) {
    std::stack<double> values;
    std::stack<char> ops;
    std::istringstream tokens(expression);
    char token;
    bool expectNegative = true;

    while (tokens >> token) {
        if (token == ' ') continue;

        if (IsNumber(token) || (token == '-' && expectNegative)) {
            tokens.putback(token);
            double value;
            tokens >> value;
            values.push(value);
            expectNegative = false;
        } else if (token == '(') {
            ops.push(token);
            expectNegative = true;
        } else if (token == ')') {
            while (!ops.empty() && ops.top() != '(') ProcessOperator(values, ops);
            
            ops.pop(); // remove '('
            expectNegative = false;
        } else if (IsOperator(token)) {
            while (!ops.empty() && Precedence(ops.top()) >= Precedence(token)) 
                ProcessOperator(values, ops);
            
            ops.push(token);
            expectNegative = true;
        }
    }

    while (!ops.empty()) ProcessOperator(values, ops);

    return values.top();
}

#endif