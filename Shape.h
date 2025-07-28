#pragma once

#include <string>
#include <vector>

class Shape {
public:
    virtual ~Shape() {}
    virtual void draw() const = 0;  
    virtual std::string toString() const = 0;
};

class Point : public Shape {
public:
    int x, y;
    Point(int x, int y);
    void draw() const override;
    std::string toString() const override;
};

class Line : public Shape {
public:
    Point start, end;
    Line(Point s, Point e);
    void draw() const override;
    std::string toString() const override;
};

