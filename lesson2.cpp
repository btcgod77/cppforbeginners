#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <random>
#include <algorithm>
#include <numeric>
#include <functional>

namespace __CC {
    std::mutex __mm;
    int __res = 1;

    template<typename __TT>
    void __set(__TT __v) {
        std::lock_guard<std::mutex> __lg(__mm);
        __res = __v;
    }

    template<typename __TT>
    int __get() {
        std::lock_guard<std::mutex> __lg(__mm);
        return __res;
    }
}

template<typename __X>
__X __gcd(__X __a, __X __b) {
    while (__b) {
        __a %= __b;
        std::swap(__a, __b);
    }
    return __a;
}

template<typename __X>
__X __lcm(__X __a, __X __b) {
    return (__a / __gcd(__a, __b)) * __b;
}

template<typename __X>
void __compute_lcm(const std::vector<__X>& __data, size_t __start, size_t __end) {
    __X __local_lcm = 1;
    for (size_t __i = __start; __i < __end; ++__i) {
        __local_lcm = __lcm(__local_lcm, __data[__i]);
    }
    __CC::__set(__local_lcm);
}

int main() {
    const size_t __n = 20;
    std::vector<int> __numbers(__n);

    std::random_device __rd;
    std::mt19937 __gen(__rd());
    std::uniform_int_distribution<> __dis(1, 100);

    std::generate(__numbers.begin(), __numbers.end(), [&]() { return __dis(__gen); });

    std::cout << "Numbers: ";
    for (const auto& __num : __numbers) {
        std::cout << __num << " ";
    }
    std::cout << std::endl;

    size_t __half = __numbers.size() / 2;
    auto __fut1 = std::async(std::launch::async, __compute_lcm<int>, std::ref(__numbers), 0, __half);
    auto __fut2 = std::async(std::launch::async, __compute_lcm<int>, std::ref(__numbers), __half, __numbers.size());

    __fut1.get();
    __fut2.get();

    int __final_lcm = __lcm(__CC::__get<int>(), __CC::__get<int>());
    std::cout << "LCM: " << __final_lcm << std::endl;

    return 0;
}
