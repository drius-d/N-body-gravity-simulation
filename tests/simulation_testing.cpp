#include "particle.hpp"
#include "simulation.hpp"
#include <iostream>
#include <chrono>
#include <limits>

int main(){
    int n;
    double dt = 0.01;
    bool running = true;

    std::cout << "Number of particles: ";
    std::cin >> n;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Simulation simulation(n, dt);

    auto previousTime = std::chrono::steady_clock::now();

    double accumulator = 0.0;

    while (true){
        auto currentTime = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;

        accumulator += elapsedTime.count();
        while (accumulator >= dt){
            simulation.updateSimulation();
            accumulator -= dt;
        }

    std::cout << "\033[2J\033[H";
    simulation.printMatrix();

    }
}