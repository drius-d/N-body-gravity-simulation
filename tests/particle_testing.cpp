#include "particle.hpp"
#include <iostream>

int main(){
    // Painfully simple test to see if the constructor runs
    Particle p1(1.0, {0.0, 0.0});

    std::cout << "Particle created successfully\n";
}