#pragma once

#include <vector>
#include <math.h>

struct Complex {
    double real;
    double imag;

    Complex(double r, double i) :
        real(r),
        imag(i) {}

    Complex(double r) :
        real(r),
        imag(0) {}

    Complex() :
        real(0),
        imag(0) {}

    void set(double r, double i) {
        real = r;
        imag = i;
    }

    void sqrt() { calcSqrt(*this); }

    double length() const { return ::sqrt(real * real + imag * imag); }

    double lengthSquared() const { return real * real + imag * imag; }

    static inline Complex sqrt(const Complex& c) {
        Complex result = c;
        calcSqrt(result);
        return result;
    }

private:
    static void calcSqrt(Complex& c) {
        double len = c.length();
        if (len > 0) {
            double y = ::sqrt((len - c.real) / 2.0);

            c.set(c.imag / (2 * y), y);
        } else {
            c.set(0, 0);
        }
    }
};

inline Complex operator*(const Complex& p, const Complex& q) {
    return Complex(p.real * q.real - p.imag * q.imag, p.real * q.imag + p.imag * q.real);
}

inline Complex operator*(const Complex& p, double scale) {
    return Complex(p.real * scale, p.imag * scale);
}

inline Complex operator/(const Complex& p, double scale) {
    return Complex(p.real / scale, p.imag / scale);
}

inline Complex operator*=(const Complex& p, double scale) {
    return Complex(p.real * scale, p.imag * scale);
}

inline Complex operator/(const Complex& p, const Complex& q) {
    Complex qConjugate(q.real, -q.imag);
    double denom = q.lengthSquared();

    return p * qConjugate / denom;
}

inline Complex operator+(const Complex& p, const Complex& q) {
    return Complex(p.real + q.real, p.imag + q.imag);
}

inline Complex operator-(const Complex& p, const Complex& q) {
    return Complex(p.real - q.real, p.imag - q.imag);
}

inline Complex operator-(const Complex& p) {
    return p * -1;
}

class Polynomial {
public:
    static std::vector<Complex> Solve(double a, double b, double c);
    static std::vector<Complex> Solve(double a, double b, double c, double d);
    static std::vector<Complex> Solve(double a, double b, double c, double d, double e);

    static std::vector<Complex> Solve(std::vector<Complex>& coeffs);
};
