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
        const double dt;                                                                            // chose not to hard-code
        std::vector<std::unique_ptr<Particle>> particles;                                           // particles created on heap since n of particles unknown              
        
        // For 3 bodies, each particle needs to know the relative position to the other 2 bodies to calculate the force vectors on it, from Newton's Law of Gravitation.
        // { {{0, 0}, {dx, dy}, {dx, dy}},
        //   {{dx, dy}, {0, 0}, {dx, dy}},
        //   {{dx, dy}, {dx, dy}, {0, 0}}
        // }

        std::vector<std::vector<std::array<double, 2>>> relativePositionsMatrix;              

    public:
        Simulation(const int n, const double dt); 

        void updateSimulation();      // updates position map, forceMap, calls particle update methods
};