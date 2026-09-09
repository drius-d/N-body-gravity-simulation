#include <vector>
#include <map>
#include <memory>
#include "particle.hpp"


class Simulation{
    private:
        const int numberOfParticles;
        const double dt;
        std::vector<std::unique_ptr<Particle>> particles;
        
        std::map<Particle*, std::vector<std::array<double, 2>>> relativePositionMap;

    public:
        Simulation(); 

        void updateSimulation();      // updates position map, forceMap, calls particle update methods
};