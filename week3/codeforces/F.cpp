#include <iostream>
#include <string>
using namespace std;
int main() {
    int n;
    cin >> n;
    const char* names[] = {"Zero", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    if (n >= 0 && n <= 9)
        cout << names[n] << endl;
    else
        cout << "Unknown" << endl;
    return 0;
}
