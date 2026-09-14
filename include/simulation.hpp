#pragma once
#include <vector>
#include <map>
#include <memory>
#include "particle.hpp"

// Simulation needs to know number of particles, time period between simulation updates, and the particles themselves
// Simulation needs a method to update itself that forces the particles to update and the information stored in simulation to update.
// Simulation is currently 2D so all vectors are 2D

class Simulation{
    private:
        const int numberOfParticles;       
        const double dt;
        const double G = 300;                                                                // universal = 6.6743e-11 but selected one that we can observe for light objects
        const double e = 1.0;
        std::vector<std::unique_ptr<Particle>> particles;                                           // particles created on heap since n of particles unknown              
        std::vector<std::array<double, 2>> previousPositions;
        
        // For 3 bodies, each particle needs to know the relative position to the other 2 bodies to calculate the force vectors on it, from Newton's Law of Gravitation.
        // { 
        //   {{0, 0}, {dx, dy}, {dx, dy}},
        //   {{dx, dy}, {0, 0}, {dx, dy}},
        //   {{dx, dy}, {dx, dy}, {0, 0}}
        // }

        std::vector<std::vector<std::array<double, 2>>> relativePositionsMatrix;              

    public:
        Simulation(const int n, const double dt); 

        void updateSimulation();      // updates position map, forceMap, calls particle update methods

        void calculateRelativePositions();

        void storePreviousPositions();

        void processCollisions();

        void forceUpdater();

        void advanceParticles(double time);

        const std::vector<std::unique_ptr<Particle>>& returnParticles() const;

        const int returnNumberOfParticles() const;

        const std::vector<std::array<double, 2>>& returnPreviousPositions() const;

        void printMatrix() const;

        std::array<double, 2> solve2x2Matrix(double a, double b, double c, double d, double e, double f) const;
};