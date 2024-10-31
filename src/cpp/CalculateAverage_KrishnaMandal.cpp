//
// Created by mfw-150 on 10/31/24.
//

#include <iostream>
#include <fstream>
#include <string>

std::string fileName = "../measurements.txt";

int main() {
    std::ifstream file(fileName); // Open the file
    if (!file) {
        std::cerr << "Unable to open file\n";
        return 1; // Return an error code
    }

    std::string line;
    while (std::getline(file, line)) { // Read the file line by line
        std::cout << line << '\n'; // Print each line
    }

    file.close(); // Close the file
    return 0;
}