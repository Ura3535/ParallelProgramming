#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include "Shared.h"

namespace ch = std::chrono;

template<typename T, typename Pred>
std::vector<T> take_where(const std::vector<T>& data, Pred pred, int num_threads = 1) {
    std::vector<T> res;

#pragma omp parallel num_threads(num_threads)
    {
        std::vector<T> local_res;
        local_res.reserve(data.size() / num_threads);

#pragma omp for
        for (int i = 0; i < data.size(); ++i)
            if (pred(data[i]))
                local_res.push_back(data[i]);

#pragma omp critical
        res.insert(res.end(), local_res.begin(), local_res.end());
    }

    return res;
}

int main() {
    const int n = 10'000'000;
    std::vector<int> data(n);
    for (int i = 0; i < n; ++i)
        data[i] = i;
    auto pred = ParallelProgShared::is_prime;


    for (int i = 1; i <= 12; ++i) {
        auto duration = ParallelProgShared::measure_time(take_where<int, decltype(pred)>, data, pred, i);
        std::cout << duration.count() / 1000.0 << ' ';
    }

    return 0;
}
