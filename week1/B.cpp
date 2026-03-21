#include <string>
using namespace std;

class Fraction {
private:
    int num;
    int den;
public:
    Fraction() : num(0), den(1) {}
    Fraction(int num, int den) : num(num), den(den) {}
    string to_string() {
        return std::to_string(num) + "/" + std::to_string(den);
    }
};

#include "main.h"
