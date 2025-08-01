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

Rectangle::Rectangle(Point tl, int w, int h) : topLeft(tl), width(w), height(h) {}

void Rectangle::draw() const {
    std::cout << "Draw Rectangle at " << topLeft.toString()
        << " with width " << width << " and height " << height << "\n";
}
std::string Rectangle::toString() const {
    return "Rectangle(" + topLeft.toString() + ", w=" + std::to_string(width) + ", h=" + std::to_string(height) + ")";
}

Circle::Circle(Point c, int r) : center(c), radius(r) {}
void Circle::draw() const {
    std::cout << "Draw Circle at " << center.toString() << " with radius " << radius << "\n";
}
std::string Circle::toString() const {
    return "Circle(" + center.toString() + ", r=" + std::to_string(radius) + ")";
}