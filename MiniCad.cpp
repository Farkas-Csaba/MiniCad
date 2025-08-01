#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include "Shape.h"
#include "SFML/Graphics.hpp"
#include "ShapeType.h"

int main() {
    std::vector<std::shared_ptr<Shape>> shapes;
    std::mutex shapeMutex;

    // State for live shape preview
    bool isDrawing = false;
    sf::Vector2f startPoint;
    ShapeType selectedShapeType = ShapeType::None;

    std::thread renderThread([&]() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "MiniCAD Viewer");

        sf::Font font;
        if (!font.loadFromFile("OpenSans-Regular.ttf")) {
            std::cerr << "Could not load font\n";
            return;
        }

        sf::Text hintText;
        hintText.setFont(font);
        hintText.setCharacterSize(16);
        hintText.setFillColor(sf::Color::Black);
        hintText.setPosition(10.f, 10.f);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case sf::Keyboard::Num1:
                        selectedShapeType = ShapeType::Point;
                        std::cout << "Mode: Point\n";
                        isDrawing = false;
                        break;
                    case sf::Keyboard::Num2:
                        selectedShapeType = ShapeType::Line;
                        std::cout << "Mode: Line\n";
                        isDrawing = false;
                        break;
                    case sf::Keyboard::Num3:
                        selectedShapeType = ShapeType::Rectangle;
                        std::cout << "Mode: Rectangle\n";
                        isDrawing = false;
                        break;
                    case sf::Keyboard::Num4:
                        selectedShapeType = ShapeType::Circle;
                        std::cout << "Mode: Circle\n";
                        isDrawing = false;
                        break;
                    default:
                        break;
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f clickPos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    std::lock_guard<std::mutex> lock(shapeMutex);

                    if (selectedShapeType == ShapeType::Point) {
                        shapes.push_back(std::make_shared<Point>(static_cast<int>(clickPos.x), static_cast<int>(clickPos.y)));
                        std::cout << "Point added at (" << clickPos.x << ", " << clickPos.y << ")\n";
                    }
                    else if (selectedShapeType == ShapeType::Line) {
                        if (!isDrawing) {
                            startPoint = clickPos;
                            isDrawing = true;
                            std::cout << "Line start point at (" << startPoint.x << ", " << startPoint.y << ")\n";
                        }
                        else {
                            shapes.push_back(std::make_shared<Line>(
                                Point(static_cast<int>(startPoint.x), static_cast<int>(startPoint.y)),
                                Point(static_cast<int>(clickPos.x), static_cast<int>(clickPos.y))
                            ));
                            std::cout << "Line completed to (" << clickPos.x << ", " << clickPos.y << ")\n";
                            isDrawing = false;
                        }
                    }
                    else if (selectedShapeType == ShapeType::Rectangle) {
                        if (!isDrawing) {
                            startPoint = clickPos;
                            isDrawing = true;
                            std::cout << "Rectangle start point at (" << startPoint.x << ", " << startPoint.y << ")\n";
                        }
                        else {
                            int left = static_cast<int>(std::min(startPoint.x, clickPos.x));
                            int top = static_cast<int>(std::min(startPoint.y, clickPos.y));
                            int width = static_cast<int>(std::abs(clickPos.x - startPoint.x));
                            int height = static_cast<int>(std::abs(clickPos.y - startPoint.y));
                            shapes.push_back(std::make_shared<Rectangle>(Point(left, top), width, height));
                            std::cout << "Rectangle completed at (" << left << ", " << top << ") size (" << width << ", " << height << ")\n";
                            isDrawing = false;
                        }
                    }
                    else if (selectedShapeType == ShapeType::Circle) {
                        if (!isDrawing) {
                            startPoint = clickPos;
                            isDrawing = true;
                            std::cout << "Circle center at (" << startPoint.x << ", " << startPoint.y << ")\n";
                        }
                        else {
                            float dx = clickPos.x - startPoint.x;
                            float dy = clickPos.y - startPoint.y;
                            int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
                            shapes.push_back(std::make_shared<Circle>(Point(static_cast<int>(startPoint.x), static_cast<int>(startPoint.y)), radius));
                            std::cout << "Circle completed with radius " << radius << "\n";
                            isDrawing = false;
                        }
                    }
                }
            }

            window.clear(sf::Color::White);

            {
                std::lock_guard<std::mutex> lock(shapeMutex);
                for (const auto& shape : shapes) {
                    if (auto p = std::dynamic_pointer_cast<Point>(shape)) {
                        sf::CircleShape circle(3.f);
                        circle.setPosition(static_cast<float>(p->x) - 3.f, static_cast<float>(p->y) - 3.f); // center circle on point
                        circle.setFillColor(sf::Color::Black);
                        window.draw(circle);
                    }
                    else if (auto l = std::dynamic_pointer_cast<Line>(shape)) {
                        sf::Vertex line[] = {
                            sf::Vertex(sf::Vector2f(static_cast<float>(l->start.x), static_cast<float>(l->start.y)), sf::Color::Blue),
                            sf::Vertex(sf::Vector2f(static_cast<float>(l->end.x), static_cast<float>(l->end.y)), sf::Color::Blue)
                        };
                        window.draw(line, 2, sf::Lines);
                    }
                    else if (auto r = std::dynamic_pointer_cast<Rectangle>(shape)) {
                        sf::RectangleShape rectShape(sf::Vector2f(static_cast<float>(r->width), static_cast<float>(r->height)));
                        rectShape.setPosition(static_cast<float>(r->topLeft.x), static_cast<float>(r->topLeft.y));
                        rectShape.setFillColor(sf::Color::Transparent);
                        rectShape.setOutlineColor(sf::Color::Green);
                        rectShape.setOutlineThickness(2.f);
                        window.draw(rectShape);
                    }
                    else if (auto c = std::dynamic_pointer_cast<Circle>(shape)) {
                        sf::CircleShape circleShape(static_cast<float>(c->radius));
                        circleShape.setPosition(static_cast<float>(c->center.x) - static_cast<float>(c->radius),
                            static_cast<float>(c->center.y) - static_cast<float>(c->radius));
                        circleShape.setFillColor(sf::Color::Transparent);
                        circleShape.setOutlineColor(sf::Color::Magenta);
                        circleShape.setOutlineThickness(2.f);
                        window.draw(circleShape);
                    }
                }
            }

            // Draw live preview for shapes with two points
            if (isDrawing) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f currentPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (selectedShapeType == ShapeType::Line) {
                    sf::Vertex tempLine[] = {
                        sf::Vertex(startPoint, sf::Color::Red),
                        sf::Vertex(currentPos, sf::Color::Red)
                    };
                    window.draw(tempLine, 2, sf::Lines);
                }
                else if (selectedShapeType == ShapeType::Rectangle) {
                    float left = std::min(startPoint.x, currentPos.x);
                    float top = std::min(startPoint.y, currentPos.y);
                    float width = std::abs(currentPos.x - startPoint.x);
                    float height = std::abs(currentPos.y - startPoint.y);

                    sf::RectangleShape rectShape(sf::Vector2f(width, height));
                    rectShape.setPosition(left, top);
                    rectShape.setFillColor(sf::Color::Transparent);
                    rectShape.setOutlineColor(sf::Color::Red);
                    rectShape.setOutlineThickness(1.f);
                    window.draw(rectShape);
                }
                else if (selectedShapeType == ShapeType::Circle) {
                    float dx = currentPos.x - startPoint.x;
                    float dy = currentPos.y - startPoint.y;
                    float radius = std::sqrt(dx * dx + dy * dy);

                    sf::CircleShape circleShape(radius);
                    circleShape.setPosition(startPoint.x - radius, startPoint.y - radius);
                    circleShape.setFillColor(sf::Color::Transparent);
                    circleShape.setOutlineColor(sf::Color::Red);
                    circleShape.setOutlineThickness(1.f);
                    window.draw(circleShape);
                }
            }

            if (selectedShapeType == ShapeType::None)
                hintText.setString("Press 1: Point | 2: Line | 3: Rect | 4: Circle");
            else if (selectedShapeType == ShapeType::Point)
                hintText.setString("Click to place a point (1-4 to change shape)");
            else if (selectedShapeType == ShapeType::Line)
                hintText.setString(isDrawing ? "Click to finish the line" : "Click to start a line");
            else if (selectedShapeType == ShapeType::Rectangle)
                hintText.setString(isDrawing ? "Click to finish the rectangle" : "Click to start a rectangle");
            else if (selectedShapeType == ShapeType::Circle)
                hintText.setString(isDrawing ? "Click to finish the circle" : "Click to start a circle");

            window.draw(hintText);
            window.display();
        }
        });

    // Command-line input (runs in main thread)
    while (true) {
        std::cout << "Commands: addpoint x y | addline x1 y1 x2 y2 | exit\n";
        std::cout << "Enter command: ";

        std::string command;
        std::cin >> command;

        if (command == "addpoint") {
            int x, y;
            std::cin >> x >> y;
            std::lock_guard<std::mutex> lock(shapeMutex);
            shapes.push_back(std::make_shared<Point>(x, y));
            std::cout << "Point added.\n";
        }
        else if (command == "addline") {
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            std::lock_guard<std::mutex> lock(shapeMutex);
            shapes.push_back(std::make_shared<Line>(Point(x1, y1), Point(x2, y2)));
            std::cout << "Line added.\n";
        }
        else if (command == "exit") {
            break;
        }
        else {
            std::cout << "Unknown command.\n";
        }
    }

    renderThread.join();

    return 0;
}
