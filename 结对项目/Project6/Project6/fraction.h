
#pragma once
#include <string>
#include <stdexcept>
#include <cmath>
#include <numeric>  // ����std::gcd
#include <algorithm> // ����std::swap

class Fraction {
    int numerator;
    int denominator;

    // �����������������Լ��
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

    // ��������
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
    // ���캯��
    Fraction(int num = 0, int den = 1) : numerator(num), denominator(den) {
        if (denominator == 0) throw std::runtime_error("��ĸ����Ϊ��");
        simplify();
    }

    // �������������
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

    // �Ƚ����������
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

    // �ַ���ת��
    std::string toString() const {
        if (denominator == 1) return std::to_string(numerator);

        int whole = numerator / denominator;
        int remainder = std::abs(numerator % denominator);

        std::string s;
        if (whole != 0) s += std::to_string(whole) + "'";
        if (remainder != 0) s += std::to_string(remainder) + "/" + std::to_string(denominator);
        return s.empty() ? "0" : s;
    }

    // �ַ�������
    static Fraction parse(const std::string& str) {
        size_t quote = str.find('\'');
        size_t slash = str.find('/');

        // �����������ʽ���� "2'3/4"��
        if (quote != std::string::npos) {
            int whole = std::stoi(str.substr(0, quote));
            size_t numStart = quote + 1;
            size_t numEnd = str.find('/', numStart);
            int num = std::stoi(str.substr(numStart, numEnd - numStart));
            int den = std::stoi(str.substr(numEnd + 1));
            return Fraction(whole * den + num, den);
        }
        // ���������ʽ���� "3/4"��
        else if (slash != std::string::npos) {
            return Fraction(std::stoi(str.substr(0, slash)),
                std::stoi(str.substr(slash + 1)));
        }
        // ����������ʽ���� "5"��
        else {
            return Fraction(std::stoi(str));
        }
    }
};