#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <random>
#include <functional>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace __AA {
    std::mutex __bb;
    std::string __cc = "Hello";
    std::string __dd = "World))";

    template<typename __TT>
    void __ff(__TT __x) {
        std::lock_guard<std::mutex> __lg(__bb);
        __cc += __x;
    }

    void __gg() {
        std::lock_guard<std::mutex> __lg(__bb);
        __cc += __dd;
    }

    std::string __hh() {
        std::lock_guard<std::mutex> __lg(__bb);
        return __cc;
    }
}

class __Builder {
public:
    __Builder(const std::string& __start) : __data(__start) {}

    void __add(const std::string& __part) {
        __data += __part;
    }

    std::string __build() {
        return __data;
    }

private:
    std::string __data;
};

template<typename __TT>
void __async_task(std::promise<__TT>&& __pr, const std::string& __msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    __pr.set_value(__msg);
}

int main() {
    std::promise<std::string> __promise1;
    std::promise<std::string> __promise2;
    std::future<std::string> __future1 = __promise1.get_future();
    std::future<std::string> __future2 = __promise2.get_future();

    std::thread __t1(__async_task<std::string>, std::move(__promise1), "Hello, ");
    std::thread __t2(__async_task<std::string>, std::move(__promise2), "World!");

    __t1.join();
    __t2.join();

    __AA::__ff(__future1.get());
    __AA::__ff(__future2.get());

    __AA::__gg();

    std::string __result = __AA::__hh();

    std::cout << __result << std::endl;

    return 0;
}
