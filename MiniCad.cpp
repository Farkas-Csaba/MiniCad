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

    // State for live line preview
    bool isDrawingLine = false;
    sf::Vector2f lineStartPoint;
    ShapeType selectedShapeType = ShapeType::None;

    // Launch SFML window in a separate thread
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
                        break;
                    case sf::Keyboard::Num2:
                        selectedShapeType = ShapeType::Line;
                        std::cout << "Mode: Line\n";
                        break;
                    case sf::Keyboard::Num3:
                        selectedShapeType = ShapeType::Rectangle;
                        std::cout << "Mode: Rectangle (not implemented yet)\n";
                        break;
                    case sf::Keyboard::Num4:
                        selectedShapeType = ShapeType::Circle;
                        std::cout << "Mode: Circle (not implemented yet)\n";
                        break;
                    default:
                        break;
                    }
                }

                //Mouse click
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f clickPos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                    std::lock_guard<std::mutex> lock(shapeMutex);

                    if (selectedShapeType == ShapeType::Point) {
                        shapes.push_back(std::make_shared<Point>(static_cast<int>(clickPos.x), static_cast<int>(clickPos.y)));
                        std::cout << "Point added at (" << clickPos.x << ", " << clickPos.y << ")\n";
                    }
                    else if (selectedShapeType == ShapeType::Line) {
                        if (!isDrawingLine) {
                            lineStartPoint = clickPos;
                            isDrawingLine = true;
                            std::cout << "Line start point at (" << lineStartPoint.x << ", " << lineStartPoint.y << ")\n";
                        }
                        else {
                            shapes.push_back(std::make_shared<Line>(
                                Point(static_cast<int>(lineStartPoint.x), static_cast<int>(lineStartPoint.y)),
                                Point(static_cast<int>(clickPos.x), static_cast<int>(clickPos.y))
                            ));
                            std::cout << "Line completed to (" << clickPos.x << ", " << clickPos.y << ")\n";
                            isDrawingLine = false;
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
                        circle.setPosition(static_cast<float>(p->x), static_cast<float>(p->y));
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
                }
            }

            // Draw live preview line if drawing
            if (isDrawingLine) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vertex tempLine[] = {
                    sf::Vertex(lineStartPoint, sf::Color::Red),
                    sf::Vertex(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)), sf::Color::Red)
                };
                window.draw(tempLine, 2, sf::Lines);
            }
            if (selectedShapeType == ShapeType::None)
                hintText.setString("Press 1: Point | 2: Line | 3: Rect | 4: Circle");
            else if (selectedShapeType == ShapeType::Point)
                hintText.setString("Click to place a point (1-4 to change shape)");
            else if (selectedShapeType == ShapeType::Line)
                hintText.setString(isDrawingLine ? "Click to finish the line" : "Click to start a line");
            else
                hintText.setString("Shape not implemented yet");

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
