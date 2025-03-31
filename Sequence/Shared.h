#pragma once

#include <vector>
#include <chrono>


namespace ParallelProgShared
{
    namespace ch = std::chrono;

    template <typename Func, typename... Args>
    ch::milliseconds measure_time(Func func, Args... args) {
        auto start_at = ch::high_resolution_clock::now();
        func(args...);
        auto finish_at = ch::high_resolution_clock::now();
        return ch::duration_cast<ch::milliseconds>(finish_at - start_at);
    }

    bool is_even(int x) {
        return !(x & 1);
    }

    bool is_prime(int x) {
        for (int i = 2; i * i <= x; ++i)
            if (x % i == 0)
                return false;

        return x > 1;
    }

    template<typename T, typename Pred>
    std::vector<T> take_where(const std::vector<T>& data, Pred pred) {
        std::vector<int> res;
		//res.reserve(data.size());

        for (const auto& item : data)
            if (pred(item))
                res.push_back(item);

        return res;
    }
}