#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    string order, type;
    cin >> order >> type;
    int n;
    cin >> n;

    if (type == "int") {
        vector<int> a(n);
        for (int i = 0; i < n; i++) cin >> a[i];
        if (order == "increase")
            sort(a.begin(), a.end());
        else
            sort(a.begin(), a.end(), greater<int>());
        for (int i = 0; i < n; i++) cout << a[i] << "\n";
    } else if (type == "char") {
        vector<char> a(n);
        for (int i = 0; i < n; i++) cin >> a[i];
        if (order == "increase")
            sort(a.begin(), a.end());
        else
            sort(a.begin(), a.end(), greater<char>());
        for (int i = 0; i < n; i++) cout << a[i] << "\n";
    } else if (type == "string") {
        vector<string> a(n);
        for (int i = 0; i < n; i++) cin >> a[i];
        if (order == "increase")
            sort(a.begin(), a.end());
        else
            sort(a.begin(), a.end(), greater<string>());
        for (int i = 0; i < n; i++) cout << a[i] << "\n";
    } else if (type == "fraction") {
        vector<pair<int,int>> a(n);
        for (int i = 0; i < n; i++) cin >> a[i].first >> a[i].second;
        auto cmp = [](const pair<int,int>& x, const pair<int,int>& y) {
            return (long long)x.first * y.second < (long long)y.first * x.second;
        };
        if (order == "increase")
            stable_sort(a.begin(), a.end(), cmp);
        else
            stable_sort(a.begin(), a.end(), [&](const pair<int,int>& x, const pair<int,int>& y) {
                return cmp(y, x);
            });
        for (int i = 0; i < n; i++) cout << a[i].first << " " << a[i].second << "\n";
    }

    return 0;
}
