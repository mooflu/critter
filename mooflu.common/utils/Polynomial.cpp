#include "Polynomial.hpp"

#include <float.h>
#include <cmath>
#include <limits>
using namespace std;

vector<Complex> Polynomial::Solve(double a, double b, double c) {
    vector<Complex> solutions;
    a = a * 2.0;
    double discriminant = b * b - 2 * a * c;
    if (discriminant >= 0.) {
        double s = sqrt(discriminant);
        solutions.push_back((-b + s) / a);
        solutions.push_back((-b - s) / a);
    } else {
        double s = sqrt(-discriminant);
        solutions.push_back(Complex(-b / a, s / a));
        solutions.push_back(Complex(-b / a, -s / a));
    }
    return solutions;
}

vector<Complex> Polynomial::Solve(double a, double b, double c, double d) {
    vector<Complex> solutions;

    double f = ((3 * c) / a - b * b / a * a) / 3;
    double g = (2 * b * b * b / (a * a * a) - 9 * b * c / (a * a) + 27 * (d / a)) / 27;
    double h = g * g / 4 + f * f * f / 27;

    if (h > 0) {
        double m = -g / 2 + sqrt(h);
        double m2 = pow(m, 1.0 / 3.0);

        double n = -g / 2 - sqrt(h);
        double n2 = pow(n, 1.0 / 3.0);

        solutions.push_back(m2 + n2 - b / (3 * a));

        Complex x2(-(m2 + n2) / 2 - b / (3 * a), (m2 - n2) * sqrt(3.0) / 2);
        Complex x3(-(m2 + n2) / 2 - b / (3 * a), -(m2 - n2) * sqrt(3.0) / 2);

        solutions.push_back(x2);
        solutions.push_back(x3);
    } else {
        double i = sqrt(g * g / 4 - h);
        double j = pow(i, 1.0 / 3.0);
        double theta = acos(-g / (2 * i));

        solutions.push_back(2 * j * cos(theta / 3) - b / (3 * a));

        double l = -j;
        double m = cos(theta / 3);
        double n = sqrt(3.0) * sin(theta / 3);
        double p = -b / (3 * a);

        solutions.push_back(l * (m + n) + p);
        solutions.push_back(l * (m - n) + p);
    }

    return solutions;
}

vector<Complex> Polynomial::Solve(double a, double b, double c, double d, double e) {
    vector<Complex> solutions;

    double bo = b;

    b = b / a;
    c = c / a;
    d = d / a;
    e = e / a;

    double f= c - 3*b*b/8;
    double g= d + b*b*b/8 - b*c/2;
    double h= e - 3*b*b*b*b/256 + b*b*c/16 - b*d/4;

    vector<Complex> cubicSolution = Solve(1, f / 2, (f * f - 4 * h) / 16, -g * g / 64);

    if (cubicSolution[1].imag != 0.) {
        Complex p(cubicSolution[1]);
        p.sqrt();

        Complex q(cubicSolution[2]);
        q.sqrt();

        Complex pq = p * q;

        Complex r(-g / (8 * pq.real));
        Complex s(bo / (4 * a));

        //the first 2 solutions are real numbers, the other 2 are complex numbers
        if (((p.imag > 0) && (q.imag > 0)) || ((p.imag < 0) && (q.imag < 0))) {
            //imag same sign
            solutions.push_back(p.real - q.real - r - s);
            solutions.push_back(-p.real + q.real - r - s);

            solutions.push_back(p - q - r - s);
            solutions.push_back(-p + q - r - s);
        } else {
            //imag different sign
            solutions.push_back(p.real + q.real + r - s);
            solutions.push_back(-p.real - q.real + r - s);

            solutions.push_back(p - q - r - s);
            solutions.push_back(-p + q - r - s);
        }
    } else {
        double y1 = cubicSolution[0].real;
        double y2 = cubicSolution[1].real;
        double y3 = cubicSolution[2].real;

        double p[3];
        int idx = 0;
        if (y1 > 0) {
            p[idx++] = sqrt(y1);
        }
        if (y2 > 0) {
            p[idx++] = sqrt(y2);
        }
        if (y3 > 0) {
            p[idx++] = sqrt(y3);
        }

        if (idx >= 2) {
            double r = -g / (8 * p[0] * p[1]);
            double s = bo / (4 * a);

            solutions.push_back(p[0] + p[1] + r - s);
            solutions.push_back(p[0] - p[1] - r - s);
            solutions.push_back(-p[0] + p[1] - r - s);
            solutions.push_back(-p[0] - p[1] + r - s);
        }
    }

    return solutions;
}

// Jenkins and Traub alg to find all the zeros of a complex polynomial.
// Based on Fortran code.

const double PI = 3.141592653589793238462643;

//  The base of the number system being used.
const double BASE = 2;

//  The number of base digits in each floating-point number (double precision)
const double kT = DBL_MANT_DIG;

//  The largest exponent in the number system.
//const double kM = DBL_MAX_EXP;

//  The smallest exponent in the number system.
const double kN = DBL_MIN_EXP;

//  The maximum relative representation error.  Fortran code:  BASE**(1-T)
const double ETA = pow(BASE, 1 - kT);

//  Infinity.  FORTRAN code:  BASE*(1.0D0-BASE**(-T))*BASE**(M-1)
const double INFIN = DBL_MAX;

//  The smallest number that can be represented.  Fortran code:  (BASE**(N+3))/BASE**3
const double SMALNO = pow(BASE, kN + 3) / pow(BASE, 3);

//  Error bounds on complex addition.
const double ARE = ETA;

//  Error bounds on complex multiplication.
const double MRE = 2 * sqrt(2.) * ETA;

// cos and sin of 94 degrees
const double COSR = cos(PI / 180.0 * 94.0);  //-0.069756474;
const double SINR = sin(PI / 180.0 * 94.0);  // 0.997564050;

static Complex PV, T, S, Z;

/**
*  Returns a scale factor to multiply the coefficients of the
*  polynomial.  The scaling is done to avoid overflow and to
*  avoid undetected underflow interfering with the convergence
*  criterion.  The factor is a power of the BASE.
*
*  @param  NN  The number of coefficients to use in the evaluation.
*  @param  P.real  The modulus of the coefficients of the polynomial.
**/
static double scale(int NN, const vector<Complex>& p) {
    // Find the largest and the smallest moduli of coefficients.
    double hi = sqrt(INFIN);
    double lo = SMALNO / ETA;
    double max = 0.;
    double min = INFIN;
    double X, sc;
    for (int i = 0; i < NN; ++i) {
        X = p[i].real;
        if (X > max) {
            max = X;
        }
        if (X != 0. && X < min) {
            min = X;
        }
    }

    // Scale only if there are very large or very small components.
    double s = 1.;
    if (min >= lo && max <= hi) {
        return s;
    }

    X = lo / min;
    if (X > 1.) {
        sc = X;
        if (INFIN / sc > max) {
            sc = 1.;
        }
    } else {
        sc = 1. / sqrt(max * min);
    }

    double L = log(sc) / log(BASE) + 0.5;
    s = pow(BASE, L);

    return s;
}

// Complex division C = A/B, avoiding overflow.
Complex divide(const Complex& a, const Complex& b) {
    if (b.real == 0. && b.imag == 0.) {
        // Division by zero, result = infinity.
        return Complex(INFIN, INFIN);
    }

    return a / b;
}

/**
*  Calculates the modulus or magnitude of a complex
*  number avoiding overflow.
*
*  Adapted from "Numerical Recipes in C: The Art of Scientific Computing"
*  2nd Edition, pg 949, ISBN 0-521-43108-5.  The NR algorithm
*  is only slightly different from the ACM algorithm, but the
*  NR version appears to be slightly more robust.
*
*  @param  c  The complex number.
**/
static double cmod(const Complex& c) {
    double ans = 0.;
    double re = abs(c.real);
    double im = abs(c.imag);

    if (re == 0.0) {
        ans = im;
    } else if (im == 0.0) {
        ans = re;
    } else if (re > im) {
        double temp = im / re;
        ans = re * sqrt(1. + temp * temp);
    } else {
        double temp = re / im;
        ans = im * sqrt(1. + temp * temp);
    }

    return ans;
}

/**
*  Evaluate this polynomial at a complex x and returns the
*  generally complex result.
*  Uses the method of Horner Recurrence.
*
*  @param  NN  The number of coefficients to use in the evaluation.
*  @param  S   The point at which to evaluate this polynomial.
*  @param  P   Coefficients of the polynomial.
*  @param  Q   Arrays to contain partial sums.
**/
Complex polyEval(int NN, const Complex& S, const vector<Complex>& p, vector<Complex>& Q) {
    Complex pv = p[0];
    Q[0] = pv;

    for (int i = 1; i < NN; ++i) {
        pv = pv * S + p[i];
        Q[i] = pv;
    }

    return pv;
}

/**
*  Bounds the error in evaluating the polynomial by
*  the method of Horner Recurrance.
*
*  @param  NN   The number of coefficients to use in the evaluation.
*  @param  Q    Partial sum from polyEval().
*  @param  MS   Modulus of the point being evaluated.
*  @param  MP   Modulus of the polynomial value.
*  @param  ARE  Error bounds on complex addition.
*  @param  MRE  Error bounds on complex multiplication.
**/
static double errEv(int NN, const vector<Complex>& Q, double MS, double MP, double ARE, double MRE) {
    double E = cmod(Q[0]) * MRE / (ARE + MRE);
    for (int i = 0; i < NN; ++i) {
        E = E * MS + cmod(Q[i]);
    }
    return (E * (ARE + MRE) - MP * MRE);
}

/**
*  Computes a lower bound on the moduli of the zeros of
*  a polynomial.
*
*  @param  NN  The number of coefficients to use in the evaluation.
*  @param  c.real  The modulus of the coefficients of the polynomial.
*  @param  c.imag  Array filled in on output.
**/
static double cauchy(int NN, vector<Complex>& c) {
    int NNm1 = NN - 1;

    c[NNm1].real = -c[NNm1].real;

    // Compute the upper estimate of bound.
    int N = NN - 1;
    int Nm1 = N - 1;
    double X = exp((log(-c[NNm1].real) - log(c[0].real)) / N);
    if (c[Nm1].real != 0.) {
        // If newton step at the origin is better, use it.
        double XM = -c[NNm1].real / c[Nm1].real;
        if (XM < X) {
            X = XM;
        }
    }
    // Chop the interval (0,X) until F <= 0.
    while (true) {
        double XM = X * 0.1;
        double F = c[0].real;
        for (int i = 1; i < NN; ++i) {
            F = F * XM + c[i].real;
        }
        if (F <= 0.) {
            break;
        }
        X = XM;
    }
    double DX = X;

    // Do newton iteration until X converges to two decimal places.
    while (abs(DX / X) > 0.005) {
        c[0].imag = c[0].real;
        for (int i = 1; i < NN; ++i) {
            c[i].imag = c[i - 1].imag * X + c[i].real;
        }
        double F = c[NNm1].imag;
        double DF = c[0].imag;
        for (int i = 1; i < N; ++i) {
            DF = DF * X + c[i].imag;
        }
        DX = F / DF;
        X = X - DX;
    }

    return X;
}

/**
*  Computes the derivative polynomial as the initial H
*  polynomial and computes L1 no-shift H polynomials.
*
*  @param  NN   The number of coefficients to use in the evaluation.
*  @param  L1   Number of Level 1 shifts to make.
*  @param  P    The coefficients of the polynomial.
*  @param  H    Array containing output
**/
void noShift(int NN, int L1, const vector<Complex>& p, vector<Complex>& H) {
    int N = NN - 1;
    int Nm1 = N - 1;
    int NNm1 = NN - 1;

    for (int i = 0; i < N; ++i) {
        double XNi = NNm1 - i;
        H[i].set(XNi * p[i].real / N, XNi * p[i].imag / N);
    }

    for (int jj = 0; jj < L1; ++jj) {
        if (cmod(H[Nm1]) > ETA * 10. * cmod(p[Nm1])) {
            // Divide the negative coefficient by the derivative.
            T = divide(-p[NNm1], H[Nm1]);
            for (int i = 1; i <= Nm1; ++i) {
                int j = NNm1 - i;
                H[j] = T * H[j - 1] + p[j];
            }
            H[0] = p[0];
        } else {
            // If the constant term is essentially zero, shift H coefficients.
            for (int i = 1; i <= Nm1; ++i) {
                int j = NNm1 - i;
                H[j] = H[j - 1];
            }
            H[0].set(0., 0.);
        }
    }
}

/**
*  Computes T = -P(S)/H(S).
*  Sets global variable T.
*
*  @param  NN    The number of coefficients to use in the evaluation.
*  @param  S     The point we are evaluating the polynomial at.
*  @param  H     Array containing ?
*  @param  QH    Array containing partial sums of H(S) polynomial.
*  @return True if H(S) is essentially zero.
**/
bool calcT(int NN, const Complex& S, const vector<Complex>& H, vector<Complex>& QH) {
    int N = NN - 1;
    int Nm1 = N - 1;

    // Evaluate H(S).
    Complex HV = polyEval(N, S, H, QH);

    // Is H(S) essentially zero?
    bool b = cmod(HV) <= ARE * 10. * cmod(H[Nm1]);
    if (b) {
        T.set(0., 0.);
    } else {
        T = divide(-PV, HV);
    }

    return b;
}

/**
*  Calculates the next shifted H polynomial.
*
*  @param  NN   The number of coefficients to use in the evaluation.
*  @param  bool Set to true if H(S) is essentially zero.
*  @param  H    Array containing ?
*  @param  QP
*  @param  QH
**/
void nextH(int NN, bool b, vector<Complex>& H, const vector<Complex>& QP, const vector<Complex>& QH) {
    int N = NN - 1;

    if (!b) {
        for (int j = 1; j < N; ++j) {
            H[j] = T * QH[j - 1] + QP[j];
        }
        H[0] = QP[0];
    } else {
        // If H(S) is zero, replace H with QH.
        for (int j = 1; j < N; ++j) {
            H[j] = QH[j - 1];
        }
        H[0].set(0., 0.);
    }
}

/**
*  Carries out the third stage iteration.
*  On entry Z contains the initial iteration.  If the iteration
*  converges it contains the final iteration on exit.  Also uses
*  and sets global variable S.
*
*  @param  NN   The number of coefficients to use in the evaluation.
*  @param  L3   Limit of steps in stage 3.
*  @param  P    The coefficients of the polynomial.
*  @param  QP
*  @param  H    Array containing ?
*  @param  QH
*  @return True if iteration converges.
**/
bool vrShift(int NN, int L3, const vector<Complex>& P, vector<Complex>& QP, vector<Complex>& H, vector<Complex>& QH) {
    bool conv = false;
    bool B = false;
    bool b = false;
    double OMP = 0., RelSTP = 0.;

    S = Z;

    // Main loop for stage three.
    for (int i = 0; i < L3; ++i) {
        // Evaluate P at S and test for convergence.
        PV = polyEval(NN, S, P, QP);

        double MP = cmod(PV);
        double MS = cmod(S);
        if (MP <= 20 * errEv(NN, QP, MS, MP, ARE, MRE)) {
            // Polynomial value is smaller in value than a bound on the error
            // in evaluating P, terminate the iteration.
            Z = S;
            return true;
        }

        if (i == 0) {
            OMP = MP;
        } else {
            if (!B && MP >= OMP && RelSTP < 0.05) {
                // Iteration has stalled.  Probably a cluster of zeros.  Do 5 fixed
                // shift steps into the cluster to force one zero to dominate.
                double TP = RelSTP;
                B = true;
                if (RelSTP < ETA) {
                    TP = ETA;
                }
                double R1 = sqrt(TP);
                double R2 = S.real * (1. + R1) - S.imag * R1;
                S.set(R2, S.real * R1 + S.imag * (1. + R1));
                PV = polyEval(NN, S, P, QP);

                for (int j = 0; j < 5; ++j) {
                    b = calcT(NN, S, H, QH);
                    nextH(NN, b, H, QP, QH);
                }

                OMP = INFIN;
            } else {
                // Exit if polynomial value increases significantly.
                if (MP * 0.1 > OMP) {
                    return conv;
                }
                OMP = MP;
            }
        }

        // Calculate next iteration.
        b = calcT(NN, S, H, QH);
        nextH(NN, b, H, QP, QH);
        b = calcT(NN, S, H, QH);
        if (!b) {
            RelSTP = cmod(T) / cmod(S);
            S = S + T;
        }
    }

    return conv;
}

/**
*  Computes L2 fixed-shift H polynomials and tests for convergence.
*  Initiates a variable-shift iteration and returns with the
*  approximate zero if successfull.
*  Uses and sets the global variable S.
*  Sets global variable Z to approximate zero if convergence is true.
*
*  @param  NN   The number of coefficients to use in the evaluation.
*  @param  L2   Limit of fixed shift steps.
*  @param  P    The coefficients of the polynomial.
*  @param  QP
*  @param  H    Array containing ?
*  @param  QH
*  @return True if convergence of stage 3 iteration is successfull.
**/
bool fxShift(int NN, int L2, const vector<Complex>& P, vector<Complex>& QP, vector<Complex>& H, vector<Complex>& QH,
             vector<Complex>& SH) {
    bool conv = false;
    int N = NN - 1;

    // Evaluate Polynomial at S.
    PV = polyEval(NN, S, P, QP);
    bool test = true;
    bool pasd = false;

    // Calculate 1st T = -P(S)/H(S).
    bool b = calcT(NN, S, H, QH);

    // Main loop for one 2nd stage step.
    for (int j = 0; j < L2; ++j) {
        Complex OT = T;

        // Compute next H polynomial and new T.
        nextH(NN, b, H, QP, QH);
        b = calcT(NN, S, H, QH);
        Z = S + T;

        // Test for convergence unless stage 3 has failed once or
        // this is the last H polynomial.
        if (!b && test && j != L2 - 1) {
            if (cmod(T - OT) < 0.5 * cmod(Z)) {
                if (pasd) {
                    // The weak convergence test has been passed twice, start
                    // the third stage iteration, after saving the current H
                    // polynomial and shift.
                    for (int i = 0; i < N; ++i) {
                        SH[i] = H[i];
                    }
                    Complex SVS = S;
                    conv = vrShift(NN, 10, P, QP, H, QH);  // Outputs Zr, Zi.
                    if (conv) {
                        return conv;
                    }

                    // The iteration failed to converge.  Turn off testing and
                    // restore H, S, PV and T.
                    test = false;
                    for (int i = 0; i < N; ++i) {
                        H[i] = SH[i];
                    }
                    S = SVS;

                    PV = polyEval(NN, S, P, QP);
                    b = calcT(NN, S, H, QH);

                } else {
                    pasd = true;
                }
            } else {
                pasd = false;
            }
        }
    }

    // Attempt an iteration with final H polynomial from second stage.
    conv = vrShift(NN, 10, P, QP, H, QH);  // Outputs Zr, Zi.

    return conv;
}

vector<Complex> Polynomial::Solve(vector<Complex>& coeffs) {
    vector<Complex> solutions;

    unsigned int NN = (unsigned int)coeffs.size();
    unsigned int degree = NN - 1;

    //  Helper arrays
    vector<Complex> H(NN);
    vector<Complex> QP(NN);
    vector<Complex> QH(NN);
    vector<Complex> SH(NN);

    double XX = sqrt(2.) / 2.;
    double YY = -XX;
    int idNN2 = 0;

    //  Remove zeros at the origin, if any.
    while (coeffs[NN - 1].real == 0. && coeffs[NN - 1].imag == 0.) {
        solutions.push_back(Complex(0., 0.));
        NN--;
        idNN2++;
    }

    //  Calculate the modulus of the coefficients.
    for (size_t i = 0; i < NN; ++i) {
        SH[i].real = cmod(coeffs[i]);
    }

    //  Scale the polynomial if needed.
    double bound = scale(NN, SH);
    if (bound != 1.0) {
        for (size_t i = 0; i < NN; ++i) {
            coeffs[i] *= bound;
        }
    }

    //  Start the algorithm for one zero.
    while (true) {
        if (NN <= 2) {
            // Calculate the final zero and return.
            T = divide(-coeffs[1], coeffs[0]);
            if (solutions.size() < (degree - 1)) {
                solutions.push_back(T);
            } else {
                solutions[degree - 1] = T;
            }

            break;
        }

        //  Calculate a lower bound on the modulus of the zeros.
        for (size_t i = 0; i < NN; ++i) {
            SH[i].real = cmod(coeffs[i]);
        }

        bound = cauchy(NN, SH);

        // Outer loop to control 2 major passes with different sequences of shifts.
        for (int cnt1 = 0; cnt1 < 2; ++cnt1) {
            // First stage calculation, no shift.
            noShift(NN, 5, coeffs, H);

            //  Inner loop to select a shift.
            for (int cnt2 = 0; cnt2 < 9; ++cnt2) {
                //  Shift is chosen with a modulus bound and amplitude rotated
                //  by 94 degrees from the previous shift.
                double XXX = COSR * XX - SINR * YY;
                YY = SINR * XX - COSR * YY;
                XX = XXX;
                S.set(bound * XX, bound * YY);

                //  Second stage calculation, fixed shift.
                bool conv = fxShift(NN, 10 * cnt2, coeffs, QP, H, QH, SH);  // Outputs Zr, Zi.
                if (conv == true) {
                    //  If successful the zero is stored and the polynomial deflated.
                    solutions.push_back(Z);
                    NN--;
                    idNN2++;

                    for (size_t i = 0; i < NN; ++i) {
                        coeffs[i] = QP[i];
                    }

                    // The 2nd stage jumps directly back to 3rd stage iteration.
                    continue;
                }

                //  If iteration is unsuccessful, another shift is chosen.
            }

            //  If 9 shifts fail, the outer loop is repeated with another
            //  sequence of shifts.
        }

        //  The zero finder has failed on two major passes.  Return empty handed.
        //throw new RootException("Found fewer than " + degree + " zeros.");
        break;
    }

    return solutions;
}
