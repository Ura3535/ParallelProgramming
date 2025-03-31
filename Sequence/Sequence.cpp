#include <iostream>
#include <execution>
#include "Shared.h"

using namespace ParallelProgShared;

int main() {
    const int n = 10'000'000;
    std::vector<int> data(n);
	std::vector<int> res(n);
    for (int i = 0; i < n; ++i)
        data[i] = i;
    auto pred = is_prime;

    auto duration_my_func = measure_time(take_where<int, decltype(pred)>, data, pred);
    auto duration_library_analog = measure_time([&data, &res, &pred]() {
        std::copy_if(data.begin(), data.end(), res.begin(), pred);
        });

    std::cout
        << duration_my_func.count() / 1000.0 << ' '
        << duration_library_analog.count() / 1000.0 << ' ';

    return 0;
}
