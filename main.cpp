#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <algorithm>
#include <Windows.h>
#include <iomanip>
#include <locale>
#include <codecvt>

#include "json.hpp"

using json = nlohmann::json;

std::string wctos(wchar_t *value);
int transform_file(std::ifstream & in_file, std::ofstream & out_file, int max_lines);
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::istringstream iss(s);
    std::string cellData;
    std::vector<std::string> cells;

    while (std::getline(iss, cellData, delimiter)) cells.push_back(cellData);

    return cells;
}

std::vector<std::wstring> wsplit(const std::string &s, char delimiter) {
    std::istringstream iss(s);
    std::string cellData;
    std::vector<std::wstring> cells;

    while (std::getline(iss, cellData, delimiter)) cells.push_back(std::wstring(cellData.begin(), cellData.end()).c_str());

    return cells;
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

void save_conf_file(json file, std::string file_name) {
    file_name = "config.json";
    std::ofstream outFile(file_name);
    outFile << file.dump(4);
    outFile.flush();
    outFile.close();
}

// Options definition
#define FILE_MENU_OPEN       1
#define FILE_MENU_SAVE       2
#define FILE_MENU_OPEN_TABLE 3
#define FILE_MENU_GEN_TABLE  4
#define FILE_MENU_EXIT       5

#define GEN_MENU_NEW         6
#define GEN_MENU_OPEN        7
#define GEN_MENU_EDIT        8
#define GEN_MENU_GEN         9

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

// 
// Line Formating Class
// 
class LineFormat {
private:
    std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> _configs;
    std::vector<int> _initials;
    std::map<std::string, CSVReader> _tables;
    int _count;

    std::string transform_date_format(const std::string& original) {
        std::istringstream iss(original);
        std::string day, month, year;

        std::getline(iss, day, '/');
        std::getline(iss, month, '/');
        std::getline(iss, year, '/');

        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << std::stoi(year) + 2000;
        oss << std::setw(2) << std::setfill('0') << month;
        oss << std::setw(2) << std::setfill('0') << day;

        return oss.str();
    }

    std::string formatValue(const std::string &value, char format) {
        std::string formattedValue = value;

        formattedValue.erase(std::remove(formattedValue.begin(), formattedValue.end(), '.'), formattedValue.end());
        std::replace(formattedValue.begin(), formattedValue.end(), ',', '.');

        // std::cout << formattedValue << std::endl;
        if (format == 'I') return std::to_string(std::stoi(formattedValue));
        else if (format == 'F') {
            double val = std::stod(formattedValue);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << val;
            return oss.str();
        }
        else if (format == 'D') return transform_date_format(value);
        else return formattedValue; 
    }

    std::string stov(const std::vector<std::string> &values, std::string value) {
        // "0", "#0#", "#@I0#"
        if (value.find("#@") != std::string::npos) {
            size_t pos = value.find("#@");
            size_t end_pos = value.find("#", pos + 2);
            char format = value[pos + 2];
            std::string index_str = value.substr(pos + 3, end_pos - pos - 3);
            // std::cout << index_str << std::endl;
            std::string value_f = formatValue(values[std::stoi(index_str)], format);
            // std::cout << value_f << std::endl;
            return value_f;
        } else if (value.find("#") != std::string::npos) {
            size_t pos = value.find('#');
            size_t end_pos = value.find('#', pos + 1);
            std::string index_str = value.substr(pos + 1, end_pos - pos - 1);
            
            // std::cout << "PRE: " << value << std::endl;
            // std::cout << "INDEX: " << index_str << std::endl;
            std::string value_f = values[std::stoi(index_str)];
            // std::cout << "POST: " << value_f << std::endl;

            return value_f;
        }
        return value;
    }

    bool parseCondition(const std::vector<std::string> &values, const std::string &condition) {
        bool condition_met = false;

        if(condition[0] == 'S') condition_met = stov(values, condition.substr(1, condition.size())).size() > 0;
        else if(condition[0] == 'G' || condition[0] == 'E') {
            std::vector<std::string> left_rigt = split(condition.substr(1, condition.size()), ',');

            std::string left  = stov(values, left_rigt[0]);
            std::string right = stov(values, left_rigt[1]);

            condition_met = condition[0] == 'G'? std::stod(left) > std::stod(right) : left == right;
        }

        return condition_met;
    }

    std::string generateNewLine_i(const std::vector<std::string> &values, const std::string &condition, const std::string &entry) {
        std::string line = entry;

        if (line[0] == 'E' || line[0] == 'G' || line[0] == 'S') {
            int pos = line.find('?');
            std::string condition = line.substr(0, pos);
            std::string after_condition = line.substr(pos + 1, line.size());
            
            return generateNewLine_i(values, condition, after_condition);
        } else if (line.find(':') != std::string::npos) {
            int pos = line.find(':');
            std::string true_statement  = line.substr(0, pos);
            std::string false_statement = line.substr(pos + 1, line.size());
            
            return generateNewLine_i(values, condition, parseCondition(values, condition)? true_statement : false_statement);
        }

        return line;
    }

    std::string generateNewLine(const std::vector<std::string> &values, const std::vector<std::string> &config) {
        std::string newLine;

        for (auto entry : config) {
            std::string line = generateNewLine_i(values, "", entry);

            if (line.find("@+") != std::string::npos) {
                int pos = line.find("@+");
                std::string str = line.substr(pos, line.size());

                int value = 0;
                char const* digits = "0123456789";
                // @1, @2, ..., @n will use the value of the _initials in n position
                if (str[0] == '@') {
                    std::size_t const n = str.find_first_of(digits, 1);
                    std::size_t const m = str.find_first_not_of(digits, n);
                    value = _initials[std::stoi(str.substr(n, m != std::string::npos ? m-n : m))];
                } else {
                    std::size_t const n = str.find_first_of(digits);
                    std::size_t const m = str.find_first_not_of(digits, n);
                    value = std::stoi(str.substr(n, m != std::string::npos ? m-n : m));
                }
                newLine += std::to_string(value + _count);
            } else if (line.find("@=") != std::string::npos) {
                int pos = line.find("@=");
                std::string str = line.substr(pos, line.size());

                char const* digits = "0123456789";
                std::size_t const n = pos + 2;
                std::size_t const m = str.find('#');
                std::string value = str.substr(n, m != std::string::npos ? m-n : m);
                
                newLine += std::to_string(_tables[value].getValue(stov(values, str.substr(m, str.size()))));
            } else if (line.find('#') != std::string::npos) {
                std::string value = line;
                
                size_t pos = value.find('#');
                while (pos != std::string::npos) {
                    size_t end_pos = value.find('#', pos + 1);
                    std::string new_value = stov(values, value.substr(pos, end_pos - pos + 1));
                    size_t size = new_value.size();
                    value.replace(pos, end_pos - pos + 1, new_value);
                    // std::cout << value << std::endl;
                    pos = value.find('#', pos + size + 1);
                }
                
                newLine += value;
            } else newLine += line;

            newLine += ";";
        }

        newLine.pop_back();

        return newLine;
    }

public:
    LineFormat(){
        _count = 0;
        _configs = {
            {"G#@I9#,0", {
            {"\"V\"", "\"A#@I0#\"", "\"#@D1#\"", "\"N/F A#@I0#\"",  "\"\"",  "0", "#@F10#",        "#2#", "\"D\"",             "\"#3#\"", "\"\"", "0", "\"\"", "\"#17# #19#\"", "\"\"", "E#20#,ESPA�A?\"\",#20#", "\"\"", "\"\"", "\"\"", "\"#3#\"", "S#15?\"#15#\",\"#16#\"", "\"Factura #@I0#\""},
            {"\"V\"", "\"A#@I0#\"", "\"#@D1#\"", "\"N/F A#@I0#\"",  "\"\"",  "0",  "#@F7#", "7000000000", "\"H\"",          "\"VENTAS\"", "\"\"", "0", "\"\"",          "\"\"", "\"\"",                    "\"\"", "\"\"", "\"\"", "\"\"",    "\"\"",                   "\"\"",              "\"\""},
            {"\"V\"", "\"A#@I0#\"", "\"#@D1#\"", "\"N/F A#@I0#\"", "\"O\"", "21",  "#@F9#", "4700000000", "\"H\"", "\"HP IVA REPERCUT\"", "\"\"", "0", "\"\"",          "\"\"", "\"\"",                    "\"\"", "\"\"", "\"\"", "\"\"",    "\"\"",                   "\"\"",              "\"\""}}},
            {"E0,0", {
            {"\"V\"", "\"A#@I0#\"", "\"#@D1#\"", "\"N/F A#@I0#\"",  "\"\"",  "0", "#@F10#",        "#2#", "\"D\"",             "\"#3#\"", "\"\"", "0", "\"\"", "\"#17# #19#\"", "\"\"", "E#20#,ESPA�A?\"\",#20#", "\"\"", "\"\"", "\"\"", "\"#3#\"", "S#15?\"#15#\",\"#16#\"", "\"Factura #@I0#\""},
            {"\"V\"", "\"A#@I0#\"", "\"#@D1#\"", "\"N/F A#@I0#\"", "\"C\"",  "0",  "#@F9#", "7000000000", "\"H\"",          "\"VENTAS\"", "\"\"", "0", "\"\"",          "\"\"", "\"\"",                    "\"\"", "\"\"", "\"\"", "\"\"",    "\"\"",                   "\"\"",              "\"\""}}}
        };
    }

    LineFormat(const std::string &configFilePath) {
        _count = 0;

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
        } else std::cerr << "Error: Unable to open config file." << std::endl;
    }

    void addInitial(int value) {
        _initials.push_back(value);
    }
    void restetInitial() {
        _initials.clear();
    }
    void addTable(std::string name, CSVReader table) {
        _tables[name] = table;
    }
    void resetTable() {
        _tables.clear();
    }

    std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> get_configs() {
        return _configs;
    }
    std::string ParseLine(std::string line){
        std::string new_lines;
        std::vector<std::string> values = split(line, ';');

        for (const auto &pair : _configs){
            std::string condition = pair.first;
            const std::vector<std::vector<std::string>> &configs = pair.second;

            if (parseCondition(values, condition)) {
                for (const auto &config : configs) {
                    std::string derivedLine = generateNewLine(values, config);
                    new_lines += derivedLine + "\n";
                }
                break;
            }
        }

        _count++;

        return new_lines;
    }
    int get_count() {
        return _count;
    }
};

// Callback definition
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateConfigEditorWindow(HWND hWnd);

// Creator function definition
void AddMenus(HWND);
void AddControlls(HWND);
void ConfigurationEditControls(HWND);
void SelectFile(HWND, std::wstring&, const wchar_t*);
void SelectConf(HWND);
void DisplayFields(HWND);

void generate_file(HWND);

// Global scope objects
HMENU hMenu;
HDC hdc;
HFONT hFont;

HWND hConfigEditorWnd = NULL;
HWND hANum;
HWND hFileSelected;
HWND hConfSelected;

CSVReader numTable;
json configuracio;

int A_NUM;
std::wstring csvFileSelected;
std::wstring cnfFileSelected;
std::wstring tabFileSelected;

// LineFormat parser;

// Main Window
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
    
    // Window Creation
    WNDCLASSW wc = {0};

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindowClass";
    wc.lpfnWndProc = WindowProcedure;

    if(!RegisterClassW(&wc)) return -1;
    CreateWindowW(L"WindowClass", L"CSV to TXT", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 600, 400, NULL, NULL, NULL, NULL);

    // Don't show console on startup
    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput =  GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.wShowWindow = SW_HIDE;

    CreateProcess(NULL, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, NULL);

    MSG msg = {0};

    // arxiu de configuracio
    std::ifstream jsonFile("files/config.json");
    jsonFile >> configuracio;
    jsonFile.close();

    std::string arxiu_previ = configuracio.value("arxiu_previ", "");
    std::string taula_clients_previ = configuracio.value("taula_clients_previ", "");
    std::string configuracio_previ = configuracio.value("configuracio_previ", "");
    int ultim_assentament = configuracio.value("ultim_assentament", 0);

    if(taula_clients_previ != "") numTable = CSVReader(taula_clients_previ);
    if(configuracio_previ  != "") cnfFileSelected = StringToWString(configuracio_previ);

    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", ultim_assentament + 1);

    SetWindowTextW(hANum, buffer);

    // Main loop
    while(GetMessage(&msg, (HWND)NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (wp){
        case FILE_MENU_OPEN:
            SelectFile(hWnd, csvFileSelected, L"Comma Separated Values\0*.CSV*\0All\0*.*\0");
            if (tabFileSelected != L"") numTable.updateNames(WStringToString(csvFileSelected), "Cliente");
            configuracio["arxiu_previ"] = WStringToString(csvFileSelected);
            save_conf_file(configuracio, "files/config.json");
            break;
        case FILE_MENU_SAVE:
            numTable.saveFile();
            std::cout << configuracio.dump(2);
            save_conf_file(configuracio, "files/config.json");
            break;
        case FILE_MENU_OPEN_TABLE:
            SelectFile(hWnd, tabFileSelected, L"Comma Separated Values\0*.CSV*\0All\0*.*\0");
            numTable = CSVReader(WStringToString(tabFileSelected));
            if (csvFileSelected != L"") numTable.updateNames(WStringToString(csvFileSelected), "Cliente");
            configuracio["taula_clients_previ"] = WStringToString(tabFileSelected);
            save_conf_file(configuracio, "files/config.json");
            break;
        case FILE_MENU_GEN_TABLE:
            if(csvFileSelected == L"" && tabFileSelected == L""){
                MessageBeep(MB_ICONEXCLAMATION);
                MessageBoxW(hWnd, L"Sel∙lecciona un arxiu origen", L"", MB_OK);
            } else numTable.updateNames(WStringToString(csvFileSelected), "Cliente");
            numTable.saveFile();
            break;
        case FILE_MENU_EXIT:
            DestroyWindow(hWnd);
            break;
        case GEN_MENU_OPEN:
            SelectFile(hWnd, cnfFileSelected, L"Text File\0*.TXT*\0All\0*.*\0");
            configuracio["configuracio_previ"] = WStringToString(cnfFileSelected);
            save_conf_file(configuracio, "files/config.json");
            // parser = LineFormat(WStringToString(cnfFileSelected));
            break;
        case GEN_MENU_EDIT:
            if(cnfFileSelected != L"") CreateConfigEditorWindow(hWnd);
            break;
        case GEN_MENU_GEN:
            generate_file(hWnd);
            break;
        default:
            break;
        }
        break;
    case WM_CREATE:
        AddMenus(hWnd);
        AddControlls(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return DefWindowProcW(hWnd, msg, wp, lp);
}

void AddMenus(HWND hWnd) {
    hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hGenMenu  = CreateMenu();

    AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_OPEN, L"Obrir Arxiu de Dades");
    AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_SAVE, L"Guardar");
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_OPEN_TABLE, L"Obrir Taula de Clients");
    AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_GEN_TABLE, L"Regenerar Taula de Clients");
    AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Sortir");

    AppendMenuW(hGenMenu, MF_STRING | MF_DISABLED, GEN_MENU_NEW, L"Nou Arxiu de Configuració");
    AppendMenuW(hGenMenu, MF_STRING, GEN_MENU_OPEN, L"Carrega Arxiu de Configuració");
    AppendMenuW(hGenMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hGenMenu, MF_STRING | MF_DISABLED, GEN_MENU_EDIT, L"Edit TXT File");
    AppendMenuW(hGenMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hGenMenu, MF_STRING, GEN_MENU_GEN, L"Generar Arixiu TXT");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Arxiu");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hGenMenu, L"Generar");
    AppendMenuW(hMenu, MF_STRING, 0, L"Ajuda");

    SetMenu(hWnd, hMenu);
}

void AddControlls(HWND hWnd) {
    HWND h = CreateWindowW(L"Static", L"Entra el primer assentament: ", WS_VISIBLE | WS_CHILD, 5, 0, 210, 20, hWnd, NULL, NULL, NULL);
    SendMessage(h, WM_SETFONT, (LPARAM)hFont, TRUE);
    hANum = CreateWindowW(L"Edit", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER, 220, 0, 60, 20, hWnd, NULL, NULL, NULL);
    SendMessage(hANum, WM_SETFONT, (LPARAM)hFont, TRUE);
}

// Define constants for control IDs
#define IDC_ADD_CONFIG_BUTTON   101
#define IDC_SAVE_CONFIG_BUTTON  102
#define IDC_CONFIG_BLOCK        103
#define IDC_ADD_LINE_BUTTON     104
#define IDC_REMOVE_BLOCK_BUTTON 105

class ConfigEditorBlock {

};

LRESULT CALLBACK ConfigEditorWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void CreateConfigEditorWindow(HWND hWnd) {
    const wchar_t class_name[] = L"ConfigEditorWindowClass";
    WNDCLASSW configEditorWc = {0};

    configEditorWc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    configEditorWc.hCursor = LoadCursor(NULL, IDC_ARROW);
    configEditorWc.hInstance = GetModuleHandle(NULL);
    configEditorWc.lpszClassName = class_name;
    configEditorWc.lpfnWndProc = ConfigEditorWindowProcedure;

    RegisterClassW(&configEditorWc);

    hConfigEditorWnd = CreateWindowW(class_name, L"Configuration Editor", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 800, 600, hWnd, NULL, NULL, NULL);

    if (hConfigEditorWnd == NULL) {
        MessageBoxW(NULL, L"Config Editor Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
}
std::vector<HWND> g_ConfigBlocks; // Store handles to configuration blocks

// Function prototypes
void AddConfigBlock(HWND);
void AddLineEditToBlock(HWND);
void RemoveConfigBlock(HWND);
void reposition_blocks();

LRESULT CALLBACK ConfigBlockWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

// Register window class for configuration block
ATOM RegisterConfigBlockClass() {
    WNDCLASSW wc = {0};

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ConfigBlock";
    wc.lpfnWndProc = ConfigBlockWindowProcedure;

    return RegisterClassW(&wc);
}

LRESULT CALLBACK ConfigBlockWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDC_ADD_LINE_BUTTON:
            // Add a new line edit below the configuration block
            AddLineEditToBlock(hWnd);
            reposition_blocks();
            break;
        case IDC_REMOVE_BLOCK_BUTTON:
            // Remove the configuration block
            RemoveConfigBlock(hWnd);
            reposition_blocks();
            break;
        }
        break;
    default:
        return DefWindowProc(hWnd, msg, wp, lp);
    }
    return 0;
}

// Window procedure for the configuration editor window
LRESULT CALLBACK ConfigEditorWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        // Add controls for modifying configurations
        ConfigurationEditControls(hWnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDC_ADD_CONFIG_BUTTON:
            // Add a new configuration block
            AddConfigBlock(hWnd);
            reposition_blocks();
            break;
        }
        break;
    case WM_DESTROY:
        // No need to call PostQuitMessage(0); to keep the main window open
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }

    return 0;
}

void ConfigurationEditControls(HWND hWnd) {
    HWND h1 = CreateWindowW(L"Button", L"Add Configuration", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 10, 5, 150, 20, hWnd, (HMENU)IDC_ADD_CONFIG_BUTTON, NULL, NULL);
    SendMessage(h1, WM_SETFONT, (LPARAM)hFont, TRUE);
    HWND h2 = CreateWindowW(L"Button", L"Save", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 165, 5, 60, 20, hWnd, (HMENU)IDC_SAVE_CONFIG_BUTTON, NULL, NULL);
    SendMessage(h2, WM_SETFONT, (LPARAM)hFont, TRUE);
}

void reposition_blocks() {
    RECT rect;
    int yPos = 30;
    for (auto i : g_ConfigBlocks) {
        GetClientRect(i, &rect);
        SetWindowPos(i, NULL, 5, yPos, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        yPos += (rect.bottom - rect.top);
    }
}

void AddConfigBlock(HWND hWndParent) {
    // Register the configuration block class if not already registered
    static bool classRegistered = false;
    if (!classRegistered) {
        RegisterConfigBlockClass();
        classRegistered = true;
    }
    
    RECT rect;
    GetClientRect(hWndParent, &rect);
    int width = rect.right - rect.left;

    // Create a configuration block window
    HWND hConfigBlock = CreateWindowW(L"ConfigBlock", L"", WS_VISIBLE | WS_CHILD , 5, 30, width - 10, 30, hWndParent, NULL, NULL, NULL);
    SendMessage(hConfigBlock, WM_SETFONT, (LPARAM)hFont, TRUE);
    if (hConfigBlock != NULL) {
        // Add line edit for configuration content
        HWND h1 = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, 5, 5, 200, 20, hConfigBlock, NULL, NULL, NULL);
        SendMessage(h1, WM_SETFONT, (LPARAM)hFont, TRUE);
        // Add button to add a new line edit below
        HWND h2 = CreateWindowW(L"Button", L"Add Line", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 205, 5, 70, 20, hConfigBlock, (HMENU)IDC_ADD_LINE_BUTTON, NULL, NULL);
        SendMessage(h2, WM_SETFONT, (LPARAM)hFont, TRUE);
        // Add button to remove the block
        HWND h3 = CreateWindowW(L"Button", L"Remove", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 280, 5, 70, 20, hConfigBlock, (HMENU)IDC_REMOVE_BLOCK_BUTTON, NULL, NULL);
        SendMessage(h3, WM_SETFONT, (LPARAM)hFont, TRUE);
        g_ConfigBlocks.push_back(hConfigBlock);
    }
}

// Function to add a new line edit within the clicked configuration block
void AddLineEditToBlock(HWND hConfigBlock) {
    RECT rect;
    GetClientRect(hConfigBlock, &rect);
    int yPos = rect.bottom, width = rect.right - rect.left;
    HWND h = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, 5, yPos, width - 20, 20, hConfigBlock, NULL, NULL, NULL);
    SendMessage(h, WM_SETFONT, (LPARAM)hFont, TRUE);
    SetWindowPos(hConfigBlock, NULL, 0, 0, rect.right, yPos + 20, SWP_NOMOVE | SWP_NOZORDER);
}

// Function to remove the clicked configuration block
void RemoveConfigBlock(HWND hConfigBlock) {
    DestroyWindow(hConfigBlock);
    g_ConfigBlocks.erase(std::remove(g_ConfigBlocks.begin(), g_ConfigBlocks.end(), hConfigBlock), g_ConfigBlocks.end());
}

void SelectFile(HWND hWnd, std::wstring &filename, const wchar_t* filter) {
    OPENFILENAMEW ofn;
    wchar_t file_name[100];

    ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;

    GetOpenFileNameW(&ofn);

    filename = ofn.lpstrFile;
}

std::string wctos(wchar_t *value) {
    std::wstring ws(value);
    std::string str(ws.begin(), ws.end());
    return str;
}

void generate_file(HWND hWnd) {
    wchar_t a_num[7], file_name[100];

    GetWindowTextW(hANum, a_num, 7);

    std::string s_file_name = WStringToString(csvFileSelected);
    std::string new_filename = s_file_name.substr(0, s_file_name.rfind(".csv")) + ".txt";
    std::string out_name = new_filename.substr(new_filename.rfind("\\") + 1, new_filename.size());

    A_NUM = std::stoi(a_num);

    std::ifstream i_file(csvFileSelected.c_str());
    std::ofstream o_file(new_filename);

    transform_file(i_file, o_file, -1);

    i_file.close();
    o_file.close();
    
    std::string msg = "Arxiu de nom \"" + out_name + "\" generat satisfactoriament";
    MessageBoxW(hWnd, std::wstring(msg.begin(), msg.end()).c_str(), L"Resultat de l'operació.", MB_OK);
}

int transform_file(std::ifstream & in_file, std::ofstream & out_file, int max_lines = -1) {
    std::string line;
    int lineCount = 0, last = max_lines == -1 ? 1 : max_lines, a_num = A_NUM;
    
    LineFormat parser(WStringToString(cnfFileSelected));
    parser.addInitial(a_num);
    parser.addTable("NCLI", numTable);

    std::getline(in_file, line);
    out_file << "\"VER013\"\n";
    while (std::getline(in_file, line) /*&& lineCount < last*/) {
        if (split(line, ';')[0] == "" || split(line, ';')[0] == "0" ) break;
        out_file << parser.ParseLine(line);
        // lineCount++;
    }

    configuracio["ultim_assentament"] = parser.get_count();
    save_conf_file(configuracio, "");

    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", parser.get_count() + 1);

    SetWindowTextW(hANum, buffer);

    return 0;
}