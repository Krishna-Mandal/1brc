#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <cfloat>
#include <thread>
#include <mutex>
#include <cmath>
#include <future>

struct CityStats {
    double min_temp;
    double max_temp;
    double total_temp;
    size_t count;

    CityStats() : min_temp(DBL_MAX), max_temp(DBL_MIN), total_temp(0), count(0) {}

    void update(double temp) {
        if (temp < min_temp) min_temp = temp;
        if (temp > max_temp) max_temp = temp;
        total_temp += temp;
        ++count;
    }

    double average() const {
        return count ? total_temp / count : 0;
    }

    static double round_to_one_decimal(double value) {
        return std::round(value * 10.0) / 10.0;
    }
};

std::mutex mtx;

void process_chunk(const std::vector<std::string>& lines, std::map<std::string, CityStats>& city_data) {
    std::map<std::string, CityStats> local_data;
    for (const auto& line : lines) {
        std::istringstream ss(line);
        std::string city;
        std::string temp_str;
        if (std::getline(ss, city, ';') && std::getline(ss, temp_str)) {
            try {
                double temp = std::stod(temp_str);
                local_data[city].update(temp);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid temperature value for city " << city << ": " << temp_str << '\n';
            }
        }
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& [city, stats] : local_data) {
        auto& global_stats = city_data[city];
        global_stats.update(stats.total_temp / stats.count);
        global_stats.min_temp = std::min(global_stats.min_temp, stats.min_temp);
        global_stats.max_temp = std::max(global_stats.max_temp, stats.max_temp);
    }
}

int main() {
    std::string filePath = "../measurements.txt";
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open file '" << filePath << "'\n";
        return 1;
    }

    std::map<std::string, CityStats> city_data;
    std::vector<std::string> lines;
    const size_t buffer_size = 10 * 1024 * 1024; // 10 MB buffer, adjust as needed
    std::vector<char> buffer(buffer_size);
    std::vector<std::future<void>> futures;

    std::string leftover;
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        std::string chunk(buffer.data(), file.gcount());
        chunk = leftover + chunk;
        leftover.clear();
        std::istringstream ss(chunk);
        std::string line;
        while (std::getline(ss, line)) {
            if (ss.eof() && chunk.back() != '\n') {
                leftover = line;
            } else {
                lines.push_back(line);
                if (lines.size() >= 1000000) { // Adjust chunk size as needed
                    futures.emplace_back(std::async(std::launch::async, process_chunk, std::move(lines), std::ref(city_data)));
                    lines.clear();
                }
            }
        }
    }

    if (!lines.empty()) {
        futures.emplace_back(std::async(std::launch::async, process_chunk, std::move(lines), std::ref(city_data)));
    }

    for (auto& future : futures) {
        future.get();
    }

    file.close();

    for (const auto& [city, stats] : city_data) {
        std::cout << city << "=" << CityStats::round_to_one_decimal(stats.min_temp)
                  << "/" << CityStats::round_to_one_decimal(stats.average())
                  << "/" << CityStats::round_to_one_decimal(stats.max_temp) << '\n';
    }

    return 0;
}
