# Convex Hull Server-Client Project
-- Authored by: Baruh Ifraimov & Dor Cohen --
## Overview

This project showcasing our knowledge in threads, implements a multi-client server and client system in C++ for managing and analyzing 2D point sets and their convex hulls. The server supports concurrent clients using threads and the proactor/reactor design patterns. Clients can send commands to create graphs, add/remove points, and request convex hull calculations.

## Features

- **Multi-client support:** Multiple clients can connect and interact with the server simultaneously.
- **Convex Hull Calculation:** Uses the Graham scan algorithm to compute the convex hull of a set of points.
- **Area Monitoring:** The server monitors the area of the convex hull and notifies when it crosses a threshold (e.g., 100 units).
- **Thread Safety:** Uses mutexes and condition variables to ensure safe concurrent access to shared data.
- **Command Interface:** Clients can send commands such as `Newgraph`, `Newpoint`, `Removepoint`, and `CH` (convex hull).

## Project Structure

```
LVL1/ - LVL10/    # Different levels/versions of the project
    main.cpp, server.cpp, client.cpp, reactor.cpp/hpp, proactor.cpp/hpp, etc.
makefile          # Main makefile to build all levels
```

## How to Build

1. Clone the repository.
2. Navigate to the project root.
3. Run:
    ```sh
    make
    ```
   Or build a specific level:
    ```sh
    cd LVL10
    make
    ```

## How to Run

1. Start the server:
    ```sh
    ./server
    ```
2. Start one or more clients (in separate terminals):
    ```sh
    ./client
    ```

## Usage

- **Newgraph N**  
  Start a new graph and input N points (format: `x,y`).
- **Newpoint x,y**  
  Add a new point to the current graph.
- **Removepoint x,y**  
  Remove a point from the current graph.
- **CH**  
  Calculate and display the convex hull area.

## Credits
- Based on Beej's Guide to Network Programming for socket handling.
- Convex hull algorithms adapted from GeeksforGeeks and Wikipedia.

## License

This project is for educational purposes.