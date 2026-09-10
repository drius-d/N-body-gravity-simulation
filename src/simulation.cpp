#include "simulation.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

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

            // Using istringstream instead because it fails
            // when it tries to read the buffer while its empty unlike cin.

            std::string input;
            std::getline(std::cin, input);

            std::istringstream stream(input);

            // If can't read from buffer into mass and position, clears buffer and 
            // forces user to input again.

            if (!(stream >> mass >> position[0] >> position[1])){
                stream.clear();
                stream.str("");

                std::cout << "Please try again: ";
                continue;
            }

            // If can't read from buffer into velocity, clears buffer and moves on
            // since velocity is not required

            if (!(stream >> velocity[0] >> velocity[1])){
                stream.clear();
                stream.str("");
                
                // lets simulation know whether it needs to pass velocity as an argument into the Particle constructor
                noVelocityInput = true;     
            }

            // In case buffer contains >6 entries, 

            stream.str("");

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

void Simulation::updateSimulation(){
    
    // clearing the matrix

    relativePositionsMatrix.clear();


    // Loops for number of particles

    for (int i = 0; i < numberOfParticles; i++){

        // defines a vector containing the i-th particle's relative positions with all particles
        // (including its relative position with itself i.e. (0,0).

        std::vector<std::array<double, 2>> relativePositions;
        std::array<double, 2> ownPosition = (*particles[i]).getPosition();

        for (int j = 0; j < numberOfParticles; j++){

            // calculates relative position between i-th particle and all particles in scope
            // and pushes it to relativePositions

            std::array<double, 2> otherPosition = (*particles[j]).getPosition();
            std::array<double, 2> relativePosition;
            
            relativePosition[0] = otherPosition[0] - ownPosition[0];
            relativePosition[1] = otherPosition[1] - ownPosition[1];

            relativePositions.push_back(relativePosition);
        }

        // creates matrix of positions. see include/simulation.hpp comments in class Simulation
        // for an idea.

        relativePositionsMatrix.push_back(relativePositions);
    }

    // Need to calculate a vector for NET force on each particle using relativePositionMap
    // F = G Mm/r^2 or in component form:
    // Fx = (k * x) / (x^2 + y^2)^(3/2), Fy = (k * y) / (x^2 + y^2)^(3/2)
    // For each particle, calculate the force vectors, calculate a net force, then
    // call particle update method 

    for (int i = 0; i < numberOfParticles; i++){
        Particle& currentParticle = (*particles[i]);
        std::array<double, 2> netForce = {0, 0};
        double massI = currentParticle.getMass();
        
        for (int j = 0; j < numberOfParticles; j++){
            if (i == j){
                continue;
            }

            const Particle& otherParticle = (*particles[j]);
            double massJ = otherParticle.getMass();
            double dx = relativePositionsMatrix[i][j][0];
            double dy = relativePositionsMatrix[i][j][1];

            double Fx = (G * massI * massJ * dx) / std::pow((dx*dx + dy*dy), 1.5);
            double Fy = (G * massI * massJ * dy) / std::pow((dx*dx + dy*dy), 1.5);

            netForce[0] += Fx;
            netForce[1] += Fy;
        }

        currentParticle.updateParticle(netForce, dt);
    }
}

void Simulation::printMatrix(){
    std::cout << "{\n";
    for (std::vector<std::array<double, 2>>& row : relativePositionsMatrix){
        std::cout << "{ ";
        for (std::array<double, 2>& position : row){
            std::cout << "{";
            std::cout << position[0] << ", " << position[1];
            std::cout << "} ";
        }
        std::cout << "}\n";
    }
    std::cout << "}";
}
