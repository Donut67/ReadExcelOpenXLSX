#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include <string>
#include <vector>
#include <codecvt>
#include <algorithm>
#include <locale>
#include <sstream>

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

#endif