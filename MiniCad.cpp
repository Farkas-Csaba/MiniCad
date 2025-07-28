#include <iostream>
#include <vector>
#include <memory>
#include "Shape.h"

int main()
{
    std::vector<std::shared_ptr<Shape>> shapes;

    while (true) {
        std::cout << "Commands: addpoint x y | addline x1 y1 x2 y2 | draw | exit\n";
        std::cout << "Enter command: ";

        std::string command;
        std::cin >> command;

        if (command == "addpoint") {
            int x, y;
            std::cin >> x >> y;
            shapes.push_back(std::make_shared<Point>(x, y));
            std::cout << "Point added.\n";
        }
}
