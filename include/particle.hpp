#pragma once
#include <array>
#include <cmath>

// particles generated in n-body collision belong to Particle and this will store their mass, position, velocity, and acceleration
// Simulation is currently 2D so all vectors are 2D

class Particle{
    private:
        double mass;
        double k = 100.0;                        // constant of proportionality between radius^3 and m 
        double radius = std::cbrt((mass / k));
        std::array<double, 2> position;       // stores x, y com    ponents
        std::array<double, 2> velocity;       // stores x, y components
        std::array<double, 2> acceleration;       // stores x, y components

    public:
        // constructor for single particle. takes mass, position, and velocity as arguments. default set to {0, 0} for velocity but not position
        // if position default set to {0, 0}, two particles can be created in the same place causing 0 separation and a zero division error for force calculated.

        Particle(const double m, const std::array<double, 2>& p, const std::array<double, 2>& v = {0, 0});    

        // simulation will call this every interval of time dt. this function calls the other update methods
        // effectively every dt, each particle's acceleration, velocity, and position are updated  

        // Simulation calculates the force between particles since it knows where each particle is, but acceleration is calculated by Particle itself

        void updateAcceleration(const std::array<double, 2>& force);

        // Acceleration -> velocity -> position is the chain of calculation.

        void updateVelocity(const double dt);

        void updatePosition(const double dt);

        std::array<double, 2>& getPosition();

        const std::array<double, 2>& getPosition() const;

        std::array<double, 2>& getVelocity();

        const std::array<double, 2>& getVelocity() const;

        double getMass() const;

        double getRadius() const;

    };