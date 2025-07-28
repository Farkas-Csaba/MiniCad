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
        else if (command == "addline") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            shapes.push_back(std::make_shared<Line>(Point(x1, y1), Point(x2, y2)));
            std::cout << "Line added.\n";
        }
        else if (command == "draw") {
            for (const auto& shape : shapes) {
                shape->draw();
            }
        }
        else if (command == "exit") {
            break;
        }

}
