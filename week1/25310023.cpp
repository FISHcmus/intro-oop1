#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
using namespace std;

int main() {
    // Task 1
    int day, month, year;
    cout << "Enter birthday (dd mm yyyy): ";
    cin >> day >> month >> year;

    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    int age = 1900 + ltm->tm_year - year;
    if (1 + ltm->tm_mon < month || (1 + ltm->tm_mon == month && ltm->tm_mday < day))
        age--;
    cout << "Your age: " << age << "\n\n";

    // Task 2
    char ch;
    cout << "Enter a character: ";
    cin >> ch;
    if (ch >= 'a' && ch <= 'z')
        ch -= 32;
    cout << "Capitalized: " << ch << "\n\n";

    // Task 3
    int amount;
    cout << "Enter amount (k): ";
    cin >> amount;

    int denom[] = {500, 200, 100, 50, 20, 10};
    cout << amount << "k = ";
    for (int i = 0; i < 6; i++) {
        if (i > 0) cout << " + ";
        cout << amount / denom[i] << " * " << denom[i] << "k";
        amount %= denom[i];
    }
    cout << "\n";

    return 0;
}
