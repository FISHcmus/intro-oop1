#include <cstdio>
using namespace std;
int main() {
    double assignment, lab, final_exam;
    int cheating;
    scanf("%lf", &assignment);
    scanf("%lf", &lab);
    scanf("%lf", &final_exam);
    scanf("%d", &cheating);
    double total = 0.0;
    if (!cheating) {
        total = 0.3 * assignment + 0.3 * lab + 0.4 * final_exam;
    }
    printf("%.1f %s\n", total, total >= 5.0 ? "PASSED" : "FAILED");
    return 0;
}
