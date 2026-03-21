#include <iostream>
#include <string>
#include <cmath>

class Fraction {
private:
    int num;
    int den;

    static int gcd(int a, int b) {
        a = abs(a); b = abs(b);
        while (b) { int t = b; b = a % b; a = t; }
        return a;
    }

public:
    Fraction() : num(0), den(1) {}
    Fraction(int num, int den) : num(num), den(den) {}

    std::string to_string() {
        return std::to_string(num) + "/" + std::to_string(den);
    }

    void simplify() {
        if (den < 0) { num = -num; den = -den; }
        int g = gcd(abs(num), abs(den));
        if (g > 0) { num /= g; den /= g; }
    }

    Fraction add(Fraction other) {
        Fraction r(num * other.den + other.num * den, den * other.den);
        r.simplify();
        return r;
    }

    Fraction subtract(Fraction other) {
        Fraction r(num * other.den - other.num * den, den * other.den);
        r.simplify();
        return r;
    }

    Fraction multiply(Fraction other) {
        Fraction r(num * other.num, den * other.den);
        r.simplify();
        return r;
    }

    Fraction divide(Fraction other) {
        Fraction r(num * other.den, den * other.num);
        r.simplify();
        return r;
    }

    int getNumerator() { return num; }
    int getDenominator() { return den; }
    void setNumerator(int num) { this->num = num; }
    void setDenominator(int den) { this->den = den; }
};

#include "main.h"
