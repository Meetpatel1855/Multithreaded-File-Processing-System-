
#include "ProcessData.h"
#include <fstream>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <numeric>

namespace seneca {

    void computeAvgFactor(const int* arr, int size, int divisor, double& avg) {
        avg = 0;
        for (int i = 0; i < size; i++) {
            avg += arr[i];
        }
        avg /= divisor;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }

    void computeVarFactor(const int* arr, int size, int divisor, double avg, double& var) {
        var = 0;
        for (int i = 0; i < size; i++) {
            var += (arr[i] - avg) * (arr[i] - avg);
        }
        var /= divisor;
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));     }

   

    ProcessData::ProcessData(const std::string& filename, int n_threads) : num_threads(n_threads) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::string("Error opening the file.");
        }

        file.read(reinterpret_cast<char*>(&total_items), sizeof(total_items));
        data = new int[total_items];
        file.read(reinterpret_cast<char*>(data), total_items * sizeof(int));

        averages = new double[num_threads]();
        variances = new double[num_threads]();
        p_indices = new int[num_threads + 1]();

        for (int i = 0; i <= num_threads; ++i) {
            p_indices[i] = i * (total_items / num_threads);
        }

        std::cout << "Item's count in file '" << filename << "': " << total_items << std::endl;
        std::cout << "  [" << data[0] << ", " << data[1] << ", " << data[2] << ", ... , "
            << data[total_items - 3] << ", " << data[total_items - 2] << ", "
            << data[total_items - 1] << "]\n";
    }

    ProcessData::~ProcessData() {
        delete[] data;
        delete[] averages;
        delete[] variances;
        delete[] p_indices;
    }

    ProcessData::operator bool() const {
        return total_items > 0 && data != nullptr && num_threads > 0 && averages && variances && p_indices;
    }

    int ProcessData::operator()(const std::string& target_file, double& avg, double& var) {
        std::vector<std::thread> threads;

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(std::bind(computeAvgFactor, data + p_indices[i], p_indices[i + 1] - p_indices[i], total_items, std::ref(averages[i])));
        }
        for (auto& t : threads) {
            t.join();
        }

        avg = std::accumulate(averages, averages + num_threads, 0.0);

        threads.clear();

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(std::bind(computeVarFactor, data + p_indices[i], p_indices[i + 1] - p_indices[i], total_items, avg, std::ref(variances[i])));
        }
        for (auto& t : threads) {
            t.join();
        }

        var = std::accumulate(variances, variances + num_threads, 0.0);

        std::ofstream outfile(target_file, std::ios::binary);
        if (!outfile) {
            throw std::string("Could not open file.");
        }
        outfile.write(reinterpret_cast<char*>(&total_items), sizeof(total_items));
        outfile.write(reinterpret_cast<char*>(data), total_items * sizeof(int));

        return 0;
    }

}
