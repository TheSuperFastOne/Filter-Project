#pragma once
#include <chrono>
#include <string>
#include <iostream>

struct ScopedTimer {
    using clock = std::chrono::steady_clock;
    const char* name;
    clock::time_point start;
    ScopedTimer(const char* n) : name(n), start(clock::now()) {}
    ~ScopedTimer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start).count();
        std::cout << name << ": " << us << " us\n";
    }
};