#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
using namespace std;

long long readLong(const string &prompt) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            size_t pos;
            long long val = stoll(line, &pos);
            if (pos != line.length())
                throw invalid_argument("trailing chars");
            return val;
        } catch (...) {
            cout << "Invalid integer. Try again.\n";
        }
    }
}

char readChar(const string &prompt) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        if (line.length() == 1)
            return line[0];
        cout << "Please enter exactly one character.\n";
    }
}

int main() {
    // Task 1
    while (true) {
        cout << "Enter birthday (dd mm yyyy): ";
        string line;
        getline(cin, line);
        istringstream iss(line);
        int day, month, year;
        if (!(iss >> day >> month >> year) || !iss.eof()) {
            cout << "Invalid input! Please enter three integers.\n";
            continue;
        }
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            cout << "Invalid date! Try again.\n";
            continue;
        }
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        int age = 1900 + ltm->tm_year - year;
        if (1 + ltm->tm_mon < month || (1 + ltm->tm_mon == month && ltm->tm_mday < day))
            age--;
        if (age < 0) {
            cout << "Birthday is in the future! Try again.\n";
            continue;
        }
        cout << "Your age: " << age << "\n\n";
        break;
    }

    // Task 2
    char ch = readChar("Enter a character: ");
    if (ch >= 'a' && ch <= 'z')
        ch -= 32;
    else if (ch >= 'A' && ch <= 'Z')
        cout << "(Already uppercase) ";
    else
        cout << "(Not a letter) ";
    cout << "Capitalized: " << ch << "\n\n";

    // Task 3
    long long amount;
    while (true) {
        amount = readLong("Enter amount (VND): ");
        if (amount > 0)
            break;
        cout << "Amount must be positive! Try again.\n";
    }

    long long orig = amount;
    cout << orig << " = ";
    long long denom[] = {500000, 200000, 100000, 50000, 20000, 10000};
    for (int i = 0; i < 6; i++) {
        if (i > 0) cout << " + ";
        cout << amount / denom[i] << " * " << denom[i];
        amount %= denom[i];
    }
    if (amount > 0)
        cout << " (remainder: " << amount << ")";
    cout << "\n";

    return 0;
}
