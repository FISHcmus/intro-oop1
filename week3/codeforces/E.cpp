#include <cstdio>
using namespace std;
int main() {
    double gpa;
    scanf("%lf", &gpa);
    if (gpa >= 8.5)
        printf("A\n");
    else if (gpa >= 7.0)
        printf("B\n");
    else if (gpa >= 5.5)
        printf("C\n");
    else if (gpa >= 4.0)
        printf("D\n");
    else
        printf("F\n");
    return 0;
}
