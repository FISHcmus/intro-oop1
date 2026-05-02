#include <cstdio>
#include <cmath>
#include <vector>
using namespace std;

double fix(double x) {
    if (fabs(x) < 1e-9) return 0.0;
    return x;
}

int main() {
    double a, b, c;
    scanf("%lf %lf %lf", &a, &b, &c);
    // ax^4 + bx^2 + c = 0, let t = x^2, solve at^2 + bt + c = 0
    vector<double> roots;
    if (fabs(a) < 1e-9) {
        // bt + c = 0 where t = x^2
        if (fabs(b) < 1e-9) {
            if (fabs(c) < 1e-9) {
                // 0 = 0, x = 0 is a solution
                roots.push_back(0.0);
            }
            // else no solution
        } else {
            double t = -c / b;
            if (t > 1e-9) {
                roots.push_back(sqrt(t));
                roots.push_back(-sqrt(t));
            } else if (fabs(t) <= 1e-9) {
                roots.push_back(0.0);
            }
        }
    } else {
        double delta = b * b - 4 * a * c;
        if (delta > 1e-9) {
            double t1 = (-b + sqrt(delta)) / (2 * a);
            double t2 = (-b - sqrt(delta)) / (2 * a);
            if (t1 > 1e-9) {
                roots.push_back(sqrt(t1));
                roots.push_back(-sqrt(t1));
            } else if (fabs(t1) <= 1e-9) {
                roots.push_back(0.0);
            }
            if (t2 > 1e-9) {
                roots.push_back(sqrt(t2));
                roots.push_back(-sqrt(t2));
            } else if (fabs(t2) <= 1e-9) {
                roots.push_back(0.0);
            }
        } else if (fabs(delta) <= 1e-9) {
            double t = -b / (2 * a);
            if (t > 1e-9) {
                roots.push_back(sqrt(t));
                roots.push_back(-sqrt(t));
            } else if (fabs(t) <= 1e-9) {
                roots.push_back(0.0);
            }
        }
    }
    if (roots.empty()) {
        printf("NO SOLUTION\n");
    } else if (roots.size() == 1) {
        printf("x1 = %.2f\n", fix(roots[0]));
    } else if (roots.size() == 2) {
        printf("x1 = %.2f, x2 = %.2f\n", fix(roots[0]), fix(roots[1]));
    } else {
        printf("x1 = %.2f, x2 = %.2f x3 = %.2f, x4 = %.2f\n", fix(roots[0]), fix(roots[1]), fix(roots[2]), fix(roots[3]));
    }
    return 0;
}
