#include <iostream>
#include <vector>
#include <future>
#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <random>

namespace __ZZ {
    std::mutex __xx1;
    std::condition_variable __xx2;
    int __xx3 = 0;
    int __xx4 = 0;

    template<typename __T>
    void __ff(__T __a, bool __b) {
        std::lock_guard<std::mutex> __lg(__xx1);
        __xx3 = __b ? (__xx3 + __a) : (__xx3 - __a);
    }

    template<typename __T>
    int __gg() {
        std::lock_guard<std::mutex> __lg(__xx1);
        if (--__xx4 == 0) {
            __xx2.notify_all();
        }
        return __xx4;
    }
}

template<typename __X>
class __YY {
public:
    __YY(__X __x, int __y) : __a(__x), __b(__y) {}

    __X __hh() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        if (__b == 0) {
            __ZZ::__ff(__a, true);
            std::cout << "Node " << __a << " (Leaf), Res: " << __ZZ::__xx3 << std::endl;
            return __a;
        }

        __X __res = __a;
        std::vector<std::future<__X>> __fs;
        for (int __i = 1; __i <= 3; ++__i) {
            __X __ch = __a * 10 + __i;
            ++__ZZ::__xx4;

            __fs.emplace_back(std::async(std::launch::async, [this, __ch]() {
                try {
                    return __YY<__X>(__ch, __b - 1).__hh();
                } catch (const std::exception& __e) {
                    std::cerr << "Async error: " << __e.what() << std::endl;
                    return __X(0);
                }
            }));
        }

        for (auto& __f : __fs) {
            try {
                __res += __f.get();
            } catch (const std::exception& __e) {
                std::cerr << "Future error: " << __e.what() << std::endl;
            }
        }

        __ZZ::__ff(__res, true);
        __ZZ::__gg<__X>();

        std::cout << "Node " << __a << " (Internal), Res: " << __ZZ::__xx3 << std::endl;
        return __res;
    }

private:
    __X __a;
    int __b;
};

template<typename __X>
__X __zz(__X __x, int __y) {
    __ZZ::__xx4 = 1;
    auto __node = __YY<__X>(__x, __y);
    auto __fut = std::async(std::launch::async, [&__node]() {
        return __node.__hh();
    });

    std::unique_lock<std::mutex> __ul(__ZZ::__xx1);
    __ZZ::__xx2.wait(__ul, [] { return __ZZ::__xx4 == 0; });

    try {
        return __fut.get();
    } catch (const std::exception& __e) {
        std::cerr << "Root error: " << __e.what() << std::endl;
        return __X(0);
    }
}

int main() {
    auto __res = __zz<int>(1, 3);
    std::cout << "Final: " << __res << std::endl;
    std::cout << "Global: " << __ZZ::__xx3 << std::endl;
    return 0;
}
