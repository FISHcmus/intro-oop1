#include <cstdio>
using namespace std;
int main() {
    double gpa;
    scanf("%lf", &gpa);
    if (gpa >= 9)
        printf("Outstanding\n");
    else if (gpa >= 8)
        printf("Excellent\n");
    else if (gpa >= 7)
        printf("Good\n");
    else if (gpa >= 6)
        printf("Above Average\n");
    else if (gpa >= 5)
        printf("Average\n");
    else
        printf("Below Average\n");
    return 0;
}
