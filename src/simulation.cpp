#include "simulation.hpp"
#include <iostream>
#include <limits>

Simulation::Simulation(const int n, const double dt)
: numberOfParticles(n), dt(dt) {


    // Loop runs for the number of particles selected and creates particles

    for (int i = 1; i <= n; i++){
        double mass;
        std::array<double, 2> position;       
        std::array<double, 2> velocity;       
        std::cout << "Specify particle's mass, position, velocity (optional).\n";
        std::cout << "Input 3-5 floats (i.e. 50.0 0.0 2.0 1.0 3.0 or 50.0 0.0 2.0): ";

        // Checks if the input is a recognised format to 
    
        bool noVelocityInput = false;
        while (true){

            // If can't read from buffer into mass and position, clears buffer and 
            // forces user to input again.

            if (!(std::cin >> mass >> position[0] >> position[1])){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Please try again: ";
                continue;
            }

            // If can't read from buffer into velocity, clears buffer and moves on
            // since velocity is not required

            if (!(std::cin >> velocity[0] >> velocity[1])){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                // lets simulation know whether it needs to pass velocity as an argument into the Particle constructor
                noVelocityInput = true;     
            }

            // In case buffer contains >6 entries, 

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            break;
        }

        if (noVelocityInput){
            std::unique_ptr<Particle> p = std::make_unique<Particle>(mass, position);
            particles.push_back(std::move(p));
        }
        else {
            std::unique_ptr<Particle> p = std::make_unique<Particle>(mass, position, velocity);
            particles.push_back(std::move(p));
        }
    }
}