#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include "Shape.h"
#include "SFML/Graphics.hpp"

int main() {
    std::vector<std::shared_ptr<Shape>> shapes;
    std::mutex shapeMutex;

    // Launch SFML window in a separate thread
    std::thread renderThread([&]() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "MiniCAD Viewer");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Mouse click to add point
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    int mx = event.mouseButton.x;
                    int my = event.mouseButton.y;

                    std::lock_guard<std::mutex> lock(shapeMutex);
                    shapes.push_back(std::make_shared<Point>(mx, my));
                    std::cout << "Point added at (" << mx << ", " << my << ") from mouse click.\n";
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

    // Wait for window thread to exit
    renderThread.join();

    return 0;
}
