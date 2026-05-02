#include <cstdio>
#include <cmath>
using namespace std;
int main() {
    double a, b;
    scanf("%lf %lf", &a, &b);
    if (fabs(a) < 1e-9) {
        if (fabs(b) < 1e-9)
            printf("INFINITE SOLUTIONS\n");
        else
            printf("NO SOLUTION\n");
    } else {
        double x = -b / a;
        printf("x = %.2f\n", x);
    }
    return 0;
}
