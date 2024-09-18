#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <random>
#include <chrono>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <sstream>

namespace __ZZ {
    std::mutex __mm;
    std::condition_variable __cv;
    std::map<std::string, int> __results;
    int __count = 0;

    template<typename __TT>
    void __set(const std::string& __key, __TT __value) {
        std::lock_guard<std::mutex> __lg(__mm);
        __results[__key] = __value;
        __count++;
        if (__count == 4) {
            __cv.notify_one();
        }
    }

    int __get(const std::string& __key) {
        std::lock_guard<std::mutex> __lg(__mm);
        return __results[__key];
    }
}

std::vector<std::string> __generate_strings(size_t __count) {
    std::vector<std::string> __vec;
    std::random_device __rd;
    std::mt19937 __gen(__rd());
    std::uniform_int_distribution<> __dist(1, 10);

    for (size_t __i = 0; __i < __count; ++__i) {
        std::string __str;
        int __len = __dist(__gen);
        for (int __j = 0; __j < __len; ++__j) {
            __str += static_cast<char>('a' + __dist(__gen) % 26);
        }
        __vec.push_back(__str);
    }
    return __vec;
}

int __transform_string(const std::string& __str) {
    int __result = 0;
    for (char __c : __str) {
        __result += (__c - 'a' + 1);
    }
    return __result * __result;
}

void __process_strings(const std::vector<std::string>& __strings, const std::string& __id) {
    int __total = 0;
    for (const auto& __str : __strings) {
        __total += __transform_string(__str);
    }
    __ZZ::__set(__id, __total);
}

int main() {
    std::vector<std::string> __strings = __generate_strings(20);
    std::cout << "Generated strings: ";
    for (const auto& __str : __strings) {
        std::cout << __str << " ";
    }
    std::cout << std::endl;

    auto __half = __strings.size() / 2;
    auto __fut1 = std::async(std::launch::async, __process_strings, std::vector<std::string>(__strings.begin(), __strings.begin() + __half), "Part1");
    auto __fut2 = std::async(std::launch::async, __process_strings, std::vector<std::string>(__strings.begin() + __half, __strings.end()), "Part2");

    std::unique_lock<std::mutex> __ul(__ZZ::__mm);
    __ZZ::__cv.wait(__ul, [] { return __ZZ::__count == 4; });

    int __result1 = __ZZ::__get("Part1");
    int __result2 = __ZZ::__get("Part2");

    int __final_result = __result1 + __result2;

    std::cout << "Final Result: " << __final_result << std::endl;

    return 0;
}
