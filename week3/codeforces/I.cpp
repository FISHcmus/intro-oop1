#include <cstdio>
#include <cmath>
using namespace std;

double fix(double x) {
    if (fabs(x) < 1e-9) return 0.0;
    return x;
}

int main() {
    double a, b, c;
    scanf("%lf %lf %lf", &a, &b, &c);
    if (fabs(a) < 1e-9) {
        // Linear equation bx + c = 0
        if (fabs(b) < 1e-9) {
            if (fabs(c) < 1e-9)
                printf("INFINITE SOLUTIONS\n");
            else
                printf("NO SOLUTION\n");
        } else {
            printf("x = %.2f\n", fix(-c / b));
        }
    } else {
        double delta = b * b - 4 * a * c;
        if (delta > 1e-9) {
            double x1 = (-b + sqrt(delta)) / (2 * a);
            double x2 = (-b - sqrt(delta)) / (2 * a);
            printf("x1 = %.2f, x2 = %.2f\n", fix(x1), fix(x2));
        } else if (fabs(delta) <= 1e-9) {
            double x = -b / (2 * a);
            printf("x = %.2f\n", fix(x));
        } else {
            printf("NO SOLUTION\n");
        }
    }
    return 0;
}
