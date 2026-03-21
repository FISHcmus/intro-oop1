#include <cmath>

class Point3D {
private:
    double x, y, z;
public:
    Point3D() : x(0), y(0), z(0) {}
    Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    double distance(Point3D other) {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }

    void move(double dx, double dy, double dz) {
        x += dx;
        y += dy;
        z += dz;
    }

    // Rotate 90 degrees counterclockwise around Oz: (x,y) -> (-y, x)
    void rotate() {
        double newX = -y;
        double newY = x;
        x = newX;
        y = newY;
    }
};

#include "main.h"
