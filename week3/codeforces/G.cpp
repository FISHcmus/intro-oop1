#include <cstdio>
#include <cctype>
using namespace std;
int main() {
    char c;
    scanf(" %c", &c);
    printf("UPPERCASE: %c\n", toupper(c));
    printf("lowercase: %c\n", tolower(c));
    return 0;
}
