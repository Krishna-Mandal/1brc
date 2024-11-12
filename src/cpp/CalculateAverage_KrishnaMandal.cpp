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
        min_temp = std::min(min_temp, temp);
        max_temp = std::max(max_temp, temp);
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

void process_chunk(const std::string& filePath, std::streampos start, std::streampos end, std::map<std::string, CityStats>& city_data) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    file.seekg(start);

    std::map<std::string, CityStats> local_data;
    std::string line;

    // Ensure we start reading from the beginning of a line
    if (start != 0) {
        std::getline(file, line);
    }

    while (file.tellg() < end && std::getline(file, line)) {
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
        global_stats.total_temp += stats.total_temp;
        global_stats.count += stats.count;
        global_stats.min_temp = std::min(global_stats.min_temp, stats.min_temp);
        global_stats.max_temp = std::max(global_stats.max_temp, stats.max_temp);
    }

    // Debug print to check if the chunk was processed
    std::cout << "Processed chunk from " << start << " to " << end << std::endl;
}

int main() {
    std::string filePath = "measurements.txt";
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open file '" << filePath << "'\n";
        std::cerr << std::filesystem::current_path() << "\n";
        return 1;
    }

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    const size_t num_threads = std::thread::hardware_concurrency();
    std::streampos chunk_size = file_size / num_threads;

    std::map<std::string, CityStats> city_data;
    std::vector<std::future<void>> futures;

    std::streampos start = 0;
    for (size_t i = 0; i < num_threads; ++i) {

        std::streampos end = (i == num_threads - 1) ? file_size : start + chunk_size;
        
        // Adjust end to the next newline character
        if (end < file_size) {
            file.seekg(end);
            std::string line;
            std::getline(file, line);
            end = file.tellg();
        }

        futures.emplace_back(std::async(std::launch::async, process_chunk, filePath, start, end, std::ref(city_data)));
        start = end;
    }

    for (auto& future : futures) {
        future.get();
    }

    file.close();

    // Debug print to check if data was aggregated
    std::cout << "Aggregated data:" << std::endl;
    for (const auto& [city, stats] : city_data) {
        std::cout << city << "=" << std::fixed << std::setprecision(1) << CityStats::round_to_one_decimal(stats.min_temp)
                  << "/" << std::fixed << std::setprecision(1) << CityStats::round_to_one_decimal(stats.average())
                  << "/" << std::fixed << std::setprecision(1) << CityStats::round_to_one_decimal(stats.max_temp) << '\n';
    }

    return 0;
}
