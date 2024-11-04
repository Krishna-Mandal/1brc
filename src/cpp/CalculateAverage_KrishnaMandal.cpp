//
// Created by mfw-150 on 10/31/24.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::string fileName = "../measurements.txt";

int main() {
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file 'measurement.txt'\n";
        return 1;
    }

    constexpr  size_t BUFFER_SIZE = 1024 * 1024; // 1 MB buffer
    std::vector<char> buffer(BUFFER_SIZE);
    std::string line;
    size_t line_count = 0;

    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        for (size_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {
                ++line_count;
                // calculate min, max, average
            }
        }
    }

    file.close();
    std::cout << "Total lines read: " << line_count << "\n";
    return 0;
}
