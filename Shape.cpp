#include "Shape.h"
#include <iostream>

Point::Point(int x_, int y_) : x(x_), y(y_) {}

void Point::draw() const {
    std::cout << "Draw Point at (" << x << ", " << y << ")\n";
}

std::string Point::toString() const {
    return "Point(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

Line::Line(Point s, Point e) : start(s), end(e) {}

void Line::draw() const {
    std::cout << "Draw Line from " << start.toString() << " to " << end.toString() << "\n";
}

std::string Line::toString() const {
    return "Line(" + start.toString() + " -> " + end.toString() + ")";
}