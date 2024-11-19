#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <limits>
#include <cstring>
#include <cfloat>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <charconv>
#include <iomanip>

double custom_strtod(const std::string& str) {
    double result;
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    
    if (ec == std::errc()) {
        return result;
    } else {
        // Handle conversion error
        std::cerr << "Conversion error for string: " << str << std::endl;
        return 0.0;
    }
}

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

void process_chunk(const char* data, size_t start, size_t end, std::unordered_map<std::string, CityStats>& city_data) {
    std::unordered_map<std::string, CityStats> local_data;
    size_t pos = start;
    std::string line;

    while (pos < end) {
        size_t line_end = pos;
        while (line_end < end && data[line_end] != '\n') {
            ++line_end;
        }
        line.assign(data + pos, line_end - pos);

        std::istringstream line_ss(line);
        std::string city;
        std::string temp_str;
        if (std::getline(line_ss, city, ';') && std::getline(line_ss, temp_str)) {
            try {
                double temp = custom_strtod(temp_str);
                local_data[city].update(temp);
            } catch (const std::exception& e) {
                std::cerr << "Error converting temperature for city " << city << ": " << temp_str << '\n';
            }
        }
        pos = line_end + 1;
    }

    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& [city, stats] : local_data) {
        auto& global_stats = city_data[city];
        global_stats.total_temp += stats.total_temp;
        global_stats.count += stats.count;
        global_stats.min_temp = std::min(global_stats.min_temp, stats.min_temp);
        global_stats.max_temp = std::max(global_stats.max_temp, stats.max_temp);
    }
}

int main() {
    const char* filePath = "measurements.txt";
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return 1;
    }

    size_t file_size = lseek(fd, 0, SEEK_END);
    const char* data = static_cast<const char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED) {
        std::cerr << "Memory mapping failed" << std::endl;
        close(fd);
        return 1;
    }

    const size_t num_threads = std::thread::hardware_concurrency();
    size_t chunk_size = file_size / num_threads;

    std::unordered_map<std::string, CityStats> city_data;
    std::vector<std::future<void>> futures;

    size_t start = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t end = (i == num_threads - 1) ? file_size : start + chunk_size;
        while (end < file_size && data[end] != '\n') {
            ++end;
        }
        futures.emplace_back(std::async(std::launch::async, process_chunk, data, start, end, std::ref(city_data)));
        start = end + 1;
    }

    for (auto& future : futures) {
        future.get();
    }

    munmap(const_cast<char*>(data), file_size);
    close(fd);

    // Collect results into a vector and sort by city name
    std::vector<std::pair<std::string, CityStats>> sorted_city_data(city_data.begin(), city_data.end());
    std::sort(sorted_city_data.begin(), sorted_city_data.end(), [](const std::pair<std::string, CityStats>& a, const std::pair<std::string, CityStats>& b) {
        return a.first < b.first;
    });

    std::cout << "Aggregated data:" << std::endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(1);

    for (const auto& [city, stats] : sorted_city_data) {
        std::cout << city << "=" << CityStats::round_to_one_decimal(stats.min_temp)
                  << "/" << CityStats::round_to_one_decimal(stats.average())
                  << "/" << CityStats::round_to_one_decimal(stats.max_temp) << '\n';
    }

    return 0;
}