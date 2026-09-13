#include "simulation.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <numeric>

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

    for (const std::unique_ptr<Particle>& p : particles){
        previousPositions.push_back(p->getPosition());
    }
}

void Simulation::updateSimulation(){
    
    // clearing the positions matrix

    relativePositionsMatrix.clear();

    // calculates relative positions matrix

    calculateRelativePositions();

    // Store previous positions as a vector so visualiser can perform interpolation

    previousPositions.clear();

    storePreviousPositions();

    // Collision checker

    collisionUpdater();

    // Need to calculate a vector for NET force on each particle using relativePositionMap

    forceUpdater();

}

void Simulation::calculateRelativePositions(){

    for (int i = 0; i < numberOfParticles; i++){

        // defines a vector containing the i-th particle's relative positions with all particles
        // (including its relative position with itself i.e. (0,0).

        std::vector<std::array<double, 2>> relativePositions;
        const std::array<double, 2> ownPosition = (*particles[i]).getPosition();

        for (int j = 0; j < numberOfParticles; j++){

            // calculates relative position between i-th particle and all particles in scope
            // and pushes it to relativePositions

            const std::array<double, 2> otherPosition = (*particles[j]).getPosition();
            std::array<double, 2> relativePosition;
            
            relativePosition[0] = otherPosition[0] - ownPosition[0];
            relativePosition[1] = otherPosition[1] - ownPosition[1];

            relativePositions.push_back(relativePosition);
        }

        // creates matrix of positions. see include/simulation.hpp comments in class Simulation
        // for an idea.

        relativePositionsMatrix.push_back(relativePositions);
    }
}

void Simulation::storePreviousPositions(){
    for (const std::unique_ptr<Particle>& p : particles){
        previousPositions.push_back(p->getPosition());
    }
}

void Simulation::collisionUpdater(){

    // Checks everyt i'th particle against every (i+1)'th particle to avoid double counting collisions

    for (int i = 0; i < particles.size(); i++){

        Particle* pParticle = particles[i].get();

        for (int j = i + 1; j < particles.size(); j++){

            Particle* pOther = particles[j].get();

            // stops if particles are the same

            if (i == j){
                continue;
            }

            // obtain distance between particles and sumOfRadii 

            std::array<double , 2>& relPosition = relativePositionsMatrix[i][j]; 
            double sumOfRadii = pParticle->getRadius() + pOther->getRadius();
            double distance = (std::sqrt(pow(relPosition[0], 2) + pow(relPosition[1], 2)));

            // check distance is 0 and skips to avoid division by 0 error

            if (distance == 0){
                continue;
            }

            // if distance <= sumOfRadii then collision occurs

            if (distance <= sumOfRadii){
                
                // calculates unit vector pointing between particle centres

                std::array<double, 2> unitVector;
                unitVector[0] = relPosition[0] / distance;
                unitVector[1] = relPosition[1] / distance;

                // gets particle masses and velocities

                double m1 = pParticle->getMass();
                double m2 = pOther->getMass();
                std::array<double, 2>& currentParticleVelocity = pParticle->getVelocity();
                std::array<double, 2>& otherParticleVelocity = pOther->getVelocity();

                // calculates component of velocity for both particles along unit vector using inner product

                double u1 = std::inner_product(currentParticleVelocity.begin(), currentParticleVelocity.end(), unitVector.begin(), 0.0);
                double u2 = std::inner_product(otherParticleVelocity.begin(), otherParticleVelocity.end(), unitVector.begin(), 0.0);
                
                // simultaneous equations consist of conservation of momentum and coefficient of restitution formula
                // c1 and c2 are to simplify the numerical values on the RHS

                double c1 = m1*u1 + m2*u2;
                double c2 = e * (u1 - u2);

                // m1*v1 + m2*v2 = c1
                // v2 - v1 = c2

                // uses Cramer's rule to solve 2x2 matrix for v1 and v2 

                std::array<double, 2> matrixValues = solve2x2Matrix(m1, m2, -1.0, 1.0, c1, c2);

                double v1 = matrixValues[0];
                double v2 = matrixValues[1];

                // updates the particle velocities with the change in velocity along the line of collision
                
                currentParticleVelocity[0] += (v1 - u1) * unitVector[0];
                currentParticleVelocity[1] += (v1 - u1) * unitVector[1];
                
                otherParticleVelocity[0] += (v2 - u2) * unitVector[0];
                otherParticleVelocity[1] += (v2 - u2) * unitVector[1];

            }

        }
    }  
}

void Simulation::forceUpdater(){

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

const std::vector<std::unique_ptr<Particle>>& Simulation::returnParticles() const{
    return particles;
}

const int Simulation::returnNumberOfParticles() const{
    return numberOfParticles;
}

const std::vector<std::array<double, 2>>& Simulation::returnPreviousPositions() const{
    return previousPositions;
}

void Simulation::printMatrix() const {
    std::cout << "{\n";
    for (const std::vector<std::array<double, 2>>& row : relativePositionsMatrix){
        std::cout << "{ ";
        for (const std::array<double, 2>& position : row){
            std::cout << "{";
            std::cout << position[0] << ", " << position[1];
            std::cout << "} ";
        }
        std::cout << "}\n";
    }
    std::cout << "}";
}

std::array<double, 2> Simulation::solve2x2Matrix(double a, double b, double c, double d, double e, double f) const{
    double det = (a * d) - (c * b);
    double det_x = (e * d) - (f * b);
    double det_y = (a * f) - (c * e);

    double x = det_x / det;
    double y = det_y / det;

    return {x, y};
}