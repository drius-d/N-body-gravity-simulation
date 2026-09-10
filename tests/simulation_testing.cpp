#include "particle.hpp"
#include "simulation.hpp"
#include <iostream>

int main(){
    int n = 2; 
    double dt = 0.001;

    Simulation simulation(n, dt);

    std::cout << "Constructor called. Particles generated\n";
}