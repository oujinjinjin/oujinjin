
#pragma once
#include <string>
#include <stdexcept>
#include <cmath>
#include <numeric>  // 用于std::gcd
#include <algorithm> // 用于std::swap

class Fraction {
    int numerator;
    int denominator;

    // 辅助函数：计算最大公约数
    static int gcd(int a, int b) {
        a = std::abs(a);
        b = std::abs(b);
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    // 分数化简
    void simplify() {
        int common = gcd(numerator, denominator);
        if (common == 0) return;
        numerator /= common;
        denominator /= common;
        if (denominator < 0) {
            numerator *= -1;
            denominator *= -1;
        }
    }

public:
    // 构造函数
    Fraction(int num = 0, int den = 1) : numerator(num), denominator(den) {
        if (denominator == 0) throw std::runtime_error("分母不能为零");
        simplify();
    }

    // 算术运算符重载
    Fraction operator+(const Fraction& other) const {
        return Fraction(
            numerator * other.denominator + other.numerator * denominator,
            denominator * other.denominator
        );
    }

    Fraction operator-(const Fraction& other) const {
        return Fraction(
            numerator * other.denominator - other.numerator * denominator,
            denominator * other.denominator
        );
    }

    Fraction operator*(const Fraction& other) const {
        return Fraction(numerator * other.numerator, denominator * other.denominator);
    }

    Fraction operator/(const Fraction& other) const {
        return Fraction(numerator * other.denominator, denominator * other.numerator);
    }

    // 比较运算符重载
    bool operator==(const Fraction& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }
    bool operator!=(const Fraction& other) const { return !(*this == other); }
    bool operator<(const Fraction& other) const {
        return numerator * other.denominator < other.numerator * denominator;
    }
    bool operator<=(const Fraction& other) const { return !(other < *this); }
    bool operator>(const Fraction& other) const { return other < *this; }
    bool operator>=(const Fraction& other) const { return !(*this < other); }

    // 字符串转换
    std::string toString() const {
        if (denominator == 1) return std::to_string(numerator);

        int whole = numerator / denominator;
        int remainder = std::abs(numerator % denominator);

        std::string s;
        if (whole != 0) s += std::to_string(whole) + "'";
        if (remainder != 0) s += std::to_string(remainder) + "/" + std::to_string(denominator);
        return s.empty() ? "0" : s;
    }

    // 字符串解析
    static Fraction parse(const std::string& str) {
        size_t quote = str.find('\'');
        size_t slash = str.find('/');

        // 处理带分数格式（如 "2'3/4"）
        if (quote != std::string::npos) {
            int whole = std::stoi(str.substr(0, quote));
            size_t numStart = quote + 1;
            size_t numEnd = str.find('/', numStart);
            int num = std::stoi(str.substr(numStart, numEnd - numStart));
            int den = std::stoi(str.substr(numEnd + 1));
            return Fraction(whole * den + num, den);
        }
        // 处理分数格式（如 "3/4"）
        else if (slash != std::string::npos) {
            return Fraction(std::stoi(str.substr(0, slash)),
                std::stoi(str.substr(slash + 1)));
        }
        // 处理整数格式（如 "5"）
        else {
            return Fraction(std::stoi(str));
        }
    }
};