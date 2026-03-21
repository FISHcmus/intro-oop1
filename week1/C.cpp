class Integer {
private:
    int value;
public:
    Integer() : value(0) {}
    Integer(int val) : value(val) {}
    int getValue() { return value; }
    void setValue(int val) { value = val; }
};

#include "main.h"
