# MiniCAD

A lightweight 2D CAD prototype written in C++ using SFML. Allows you to draw basic shapes with live preview and multithreaded rendering.(in early stages of development)

## Features

- Add points, lines, rectangles, circles with mouse clicks
- Live preview of shapes before committing
- Command-line shape input
- Uses SFML for graphics
- Multithreaded architecture (render + input separated)

## How to Build

1. **Requirements**
   - Visual Studio 2022
   - SFML 2.6.2 (dynamic linking)
   - Windows x64
     

2. **Setup**
   - Link SFML’s `lib` and `include` directories in project settings
   - Copy `.dll` files from `SFML/bin` into your build output folder (e.g. `x64/Release`)
   - Set build platform to `x64`
   - place OpenSans-Regular.ttf in your working directory

3. **Build & Run**
   - Open the `.sln` file in Visual Studio
   - Press `Ctrl + F5` to build and run
