#pragma once
// geeksforgeeks

#include <cstdint>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include <atomic>

class HyperLogLog {
public:
    explicit HyperLogLog(uint8_t p = 14)
        : p_(p),
          m_(uint32_t(1u) << p), // 2^p
          registers_(m_),
          alpha_m_(calc_alpha(m_)) {
            for (auto& r : registers_) {
            r.store(0);
            }
          }

    template <typename T>
    void add(const T& value) {
        uint64_t h = std::hash<T>{}(value); //convert value to number
        uint64_t x = hashKey(h);
        uint32_t idx = uint32_t(x >> (64 - p_));
        uint64_t w = x << p_;
        uint8_t zeros = leading_zeros_in_lower_bits(w, 64 - p_) + 1;
        
        auto& cell = registers_[idx];
        auto expected = cell.load();
        do {
            if (zeros <= expected) break;
        } while (!cell.compare_exchange_strong(expected, zeros));
    }

    void add(const std::string& s) { add<std::string>(s); } //correct C-string handling
    void add(const char* s) { add<std::string>(std::string(s)); }

    double estimate() const {
        double sum = 0.0;
        for (uint8_t r : registers_) {
            sum += std::ldexp(1.0, -int(r));
        }
        return alpha_m_ * (double(m_) * double(m_)) / sum;
    }

private:
    uint8_t p_;
    uint32_t m_;
    std::vector<std::atomic<uint8_t>> registers_;
    double alpha_m_;

    static double calc_alpha(uint32_t m) {
        if (m == 16)  return 0.673;
        if (m == 32)  return 0.697;
        if (m == 64)  return 0.709;
        return 0.7213 / (1.0 + 1.079 / m);
    }

    static constexpr uint64_t hashKey(uint64_t k) {
        const uint64_t m = 0xc6a4a7935bd1e995ULL;
        const int r = 47;
        uint64_t h = 0x8445d61a4e774912ULL ^ (8ULL * m);
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
        h ^= h >> r;
        h *= m;
        h ^= h >> r;
        return h;
    }

    static uint8_t leading_zeros_in_lower_bits(uint64_t w, int bits) {
        uint8_t cnt = 0;
        for (int i = bits - 1; i >= 0; --i) {
            if ((w >> i) & 1ULL) break;
            ++cnt;
        }
        return cnt;
    }
};
