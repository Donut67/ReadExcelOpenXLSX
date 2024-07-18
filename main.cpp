// ===== External Includes ===== //
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

#include <commdlg.h>
#include <shlwapi.h>
#include <filesystem>

// ===== Include OpenXLSX ===== //
#include <OpenXLSX.hpp>

// ===== Include Help Functions ===== //
#include "csv_reader.h"
#include "parse_configurations.h"
#include "help_functions.h"

#include "json.hpp"
using json = nlohmann::json;

using namespace OpenXLSX;

// ===== Static Definition ===== //
#define GEN_VENTES      10
#define GEN_ABONAMENTS  11
#define GEN_COMPRES_WOP 12

// ===== Functions ===== //
void AddControlls(HWND);
int SelectFile(HWND hWnd, std::wstring &filename, const wchar_t* filter);
void ParseFile(HWND hWnd, const std::string &config_file, std::wstring file);
bool IsCumulativeSumRow(const XLRow& row);

void SaveConfigurationFile(json file, std::string file_name);

// ===== Callback definition ===== //
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// ===== Global scope objects ===== //
HMENU hMenu;
HDC hdc;
HFONT hFont;

HWND hConfigEditorWnd = NULL;
HWND hANum;
HWND hFileSelected;
HWND hConfSelected;

CSVReader numTable;
json configuracio;

// CSVReader numTable;
// json configuracio;

std::filesystem::path currentDir;

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

    currentDir = std::filesystem::current_path();

    // arxiu de configuracio
    std::wstring filesDirectory = currentDir / L"config.json";
    std::ifstream jsonFile(WStringToString(filesDirectory));
    jsonFile >> configuracio;
    jsonFile.close();

    int primer_assentament = configuracio.value("primer_assentament", 0);

    std::string clientFileSelected = WStringToString(currentDir / L"NumClients.csv");
    if(clientFileSelected != "") numTable = CSVReader(clientFileSelected);

    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", primer_assentament + 1);

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
        case GEN_VENTES: {
            std::wstring file;
            if(SelectFile(NULL, file, L"Excel (xls)\0*.XLS*\0Excel (xlsx)\0*.XLSX*\0All\0*.*\0")) break;

            numTable.updateNamesXLSX(WStringToString(file), "Cliente");
            ParseFile(hWnd, (currentDir / L"config" / L"conf_ventes.txt").string(), file); 
            break;
        }
        case GEN_ABONAMENTS: {
            std::wstring file;
            if(SelectFile(NULL, file, L"Excel (xls)\0*.XLS*\0Excel (xlsx)\0*.XLSX*\0All\0*.*\0")) break;

            numTable.updateNamesXLSX(WStringToString(file), "Cliente");
            ParseFile(hWnd, (currentDir / L"config" / L"conf_abonaments.txt").string(), file); 
            break;
        }
        case GEN_COMPRES_WOP: {
            std::wstring file;
            if(SelectFile(NULL, file, L"Excel (xls)\0*.XLS*\0Excel (xlsx)\0*.XLSX*\0All\0*.*\0")) break;

            ParseFile(hWnd, (currentDir / L"config" / L"conf_compres_wop.txt").string(), file); 
            break;
        }
        }
        break;
    case WM_CREATE:
        // AddMenus(hWnd);
        AddControlls(hWnd);
        break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: break;
    }

    return DefWindowProcW(hWnd, msg, wp, lp);
}

void AddControlls(HWND hWnd) {
    // Assentaments
    HWND h = CreateWindowW(L"Static", L"Primer assentament: ", WS_VISIBLE | WS_CHILD, 5, 5, 210, 20, hWnd, NULL, NULL, NULL);
    SendMessage(h, WM_SETFONT, (LPARAM)hFont, TRUE);
    hANum = CreateWindowW(L"Edit", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER, 220, 5, 60, 20, hWnd, NULL, NULL, NULL);
    SendMessage(hANum, WM_SETFONT, (LPARAM)hFont, TRUE);

    // Controls
    HWND hc = CreateWindowW(L"Static", L"Controls de generació d'arxius: ", WS_VISIBLE | WS_CHILD, 5, 30, 210, 20, hWnd, NULL, NULL, NULL);
    SendMessage(hc, WM_SETFONT, (LPARAM)hFont, TRUE);
    HWND h1 = CreateWindowW(L"Button", L"Ventes", WS_VISIBLE | WS_CHILD, 5, 55, 100, 20, hWnd, (HMENU)GEN_VENTES, NULL, NULL);
    SendMessage(h1, WM_SETFONT, (LPARAM)hFont, TRUE);
    HWND h2 = CreateWindowW(L"Button", L"Abonaments", WS_VISIBLE | WS_CHILD, 110, 55, 100, 20, hWnd, (HMENU)GEN_ABONAMENTS, NULL, NULL);
    SendMessage(h2, WM_SETFONT, (LPARAM)hFont, TRUE);
    HWND h3 = CreateWindowW(L"Button", L"Compres WOP", WS_VISIBLE | WS_CHILD, 215, 55, 100, 20, hWnd, (HMENU)GEN_COMPRES_WOP, NULL, NULL);
    SendMessage(h3, WM_SETFONT, (LPARAM)hFont, TRUE);

}

void ParseFile(HWND hWnd, const std::string &config_file, std::wstring file) {
    wchar_t a_num[10];

    GetWindowTextW(hANum, a_num, 10);

    std::wstring extension;
    extension = file.substr(file.rfind(L".") + 1, file.size());

    std::string s_file_name = WStringToString(file);
    std::string new_filename = s_file_name.substr(0, s_file_name.rfind(".") + 1) + "txt";
    std::ofstream out_file(new_filename);
    out_file << "\"VER013\"\n";

    XLDocument doc;
    std::map<std::string, CSVReader*> sheets;

    doc.open(WStringToString(file));
    auto wks = doc.workbook().worksheet("A");

    sheets.insert(std::pair<std::string, CSVReader*>("NCLI", &numTable));

    ParseConfigurations parser(config_file, sheets, std::stoi(a_num));

    for(auto row : wks.rows()) {
        // if(row.rowNumber() == 50) break;
        if(row.rowNumber() != 1) {
            bool finish = false;

            std::ostringstream oss;
            for(auto cell : row.cells()) {
                if (cell.formula().get().find("SUM") != std::string::npos) {
                    finish = true;
                    break;
                }
                oss << cell.value() << ";";
            }
            if(finish) break;
            out_file << parser.ParseLine(oss.str() + "\n");
        }
    }

    doc.close();
    out_file.close();

    std::string msg = "Arxiu de nom \"" + new_filename + "\" generat satisfactoriament";
    MessageBoxW(hWnd, std::wstring(msg.begin(), msg.end()).c_str(), L"Resultat de l'operació.", MB_OK);

    // Save results
    configuracio["primer_assentament"] = parser.get_count() + 1;
    SaveConfigurationFile(configuracio, (currentDir / L"config.json").u8string());

    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", parser.get_count() + 1);
    SetWindowTextW(hANum, buffer);
}

bool IsCumulativeSumRow(const XLRow& row) {
    for (const auto& cell : row.cells()) {
        // Assuming cumulative sum rows contain the word "SUM" in one of the cells.
        if (cell.value().type() == XLValueType::String) {
            std::string cellValue = cell.value().get<std::string>();
            if (cellValue.find("SUM") != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

int SelectFile(HWND hWnd, std::wstring &filename, const wchar_t* filter) {
    OPENFILENAMEW ofn;
    wchar_t file_name[MAX_PATH];

    ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;

    if(GetOpenFileNameW(&ofn)) filename = ofn.lpstrFile;
    else return 1;

    return 0;
}

void SaveConfigurationFile(json file, std::string file_name) {
    std::ofstream outFile(file_name);
    outFile << file.dump(4);
    outFile.flush();
    outFile.close();
}