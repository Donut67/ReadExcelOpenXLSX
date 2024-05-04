#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <algorithm>
#include <Windows.h>
#include <iomanip>
#include <locale>

int get_file(std::ifstream & file, std::string &file_name);
std::string concatenateWithSeparator(const std::vector<std::string>& strings, char separator);
int transform_file(std::ifstream & in_file, std::ofstream & out_file, int max_lines);

int main() {
    // Open Excel file
    std::ifstream file;
    std::string filename;
    if (!get_file(file, filename)) {
        std::cerr << "Error: Unable to open input Excel file." << std::endl;
        return 1;
    }

    std::string new_filename = filename.substr(0, filename.rfind(".csv")) + ".txt";

    // Open output text file
    std::ofstream textFile(new_filename);
    if (!textFile) {
        std::cerr << "Error: Unable to create output text file." << std::endl;
        return 1;
    }

    // Read Excel data and write to text file
    if (transform_file(file, textFile, -1)) {
        std::cerr << "Error: Unable to transform the data." << std::endl;
        return 1;
    }

    // Close files
    file.close();
    textFile.close();

    MessageBox(NULL, "Excel data transformed and saved to output.txt successfully", "Result", MB_OK);

    return 0;
}

int get_file(std::ifstream & file, std::string &file_name) {
    OPENFILENAME ofn;
    char file_name_[100];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFile = file_name_;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 2048;
    ofn.lpstrFilter = "All\0*.*\0Comma Separated Values\0*.CSV*\0";
    ofn.nFilterIndex = 2;

    if(GetOpenFileName(&ofn)) file.open(ofn.lpstrFile, std::ios_base::in);

    file_name = ofn.lpstrFile;

    return file.is_open();
}

std::string concatenateWithSeparator(const std::vector<std::string>& strings, char separator = ';') {
    std::string result;

    for (size_t i = 0; i < strings.size(); ++i) {
        result += strings[i];
        if (i < strings.size() - 1) result += separator;
    }

    return result;
}

int transform_file(std::ifstream & in_file, std::ofstream & out_file, int max_lines = -1) {
    std::string line;
    int lineCount = 0, last = max_lines == -1 ? 1 : max_lines;
    int progressBarPosition = 0;

    std::getline(in_file, line);

    while (std::getline(in_file, line) && lineCount < last) {
        std::istringstream iss(line);
        std::string cellData;
        std::vector<std::string> cells;

        // Split line into cells
        while (std::getline(iss, cellData, ';')) cells.push_back(cellData);
        
        if(cells[0] != "0") {
            // Transform the data
            cells[0].erase(std::remove(cells[0].begin(), cells[0].end(), '.'), cells[0].end());
        
            // Convert cells[9] to integer (assuming comma as decimal separator)
            cells[9].erase(std::remove(cells[9].begin(), cells[9].end(), '.'), cells[9].end());
            std::replace(cells[9].begin(), cells[9].end(), ',', '.');

            std::vector<std::string> line1(40, "\"\"");
            std::vector<std::string> line2(40, "\"\"");
            std::vector<std::string> line3(40, "\"\"");

            line1[0] = line2[0] = line3[0] = "\"V\"";
            line1[1] = line2[1] = line3[1] = "\"A" + cells[0] + "\"";
            line1[2] = line2[2] = line3[2] = cells[1];
            line1[3] = line2[3] = line3[3] = "\"N/F A" + cells[0] + "\"";
            line1[6] = "\"" + cells[10] + "\"";
            line1[7] = line2[7] = line3[7] = "\"" + cells[2] + "\"";
            line1[8] = "\"D\"";
            line1[13] = "\"" + cells[17] + " " + cells[19] + "\"";
            line1[15] = "\"" + cells[20] + "\"";
            line1[9] = line1[19] = "\"" + cells[3] + "\"";
            line1[20] = "\"" + cells[15] + "\"";
            line1[21] = "\"Factura " + cells[0] + "\"";

            line1[5] = line1[11] = line1[27] = line1[28] = line1[35] = 
                line2[5] = line2[11] = line2[25] = line2[27] = line2[28] = line2[35] = 
                line3[11] = line3[27] = line3[28] = line3[35] = "\"0\"";

            line2[6] = "\"" + cells[7] + "\"";
            line2[8] = "\"H\"";
            line2[9] = "\"VENTAS\"";

            line3[4] = (stod(cells[9]) > 0 ? "\"C\"" : "\"O\"");
            line3[5] = (stod(cells[9]) > 0 ?  "21" : "0");
            line3[6] = "\"" + cells[9] + "\"";
            line3[8] = "\"H\"";
            line3[9] = "\"HP IVA REPERCUT\"";

            // Write transformed lines to output file
            out_file << concatenateWithSeparator(line1) << "\n";
            out_file << concatenateWithSeparator(line2) << "\n";
            out_file << concatenateWithSeparator(line3) << "\n";

            if(max_lines == -1) last++;
            lineCount++;
        } else break;
    }

    return 0;
}