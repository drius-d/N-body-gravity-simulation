#include <array>

// particles generated in n-body collision belong to Particle and this will store their mass, position, velocity, and acceleration
// Simulation is currently 2D so all vectors are 2D
class Particle{
    private:
        double mass;
        std::array<double, 2> position;       // stores x, y components
        std::array<double, 2> velocity;       // stores x, y components
        std::array<double, 2> acceleration;       // stores x, y components

    public:
        // constructor for single particle. takes mass, position, and velocity as arguments. default set to {0, 0} for velocity but not position
        // if position default set to {0, 0}, two particles can be created in the same place causing 0 separation and a zero division error for force calculated.
        Particle(double m, std::array<double, 2> p, std::array<double, 2> v = {0, 0});    

        // simulation will call this every interval of time dt. this function calls the other update methods
        // effectively every dt, each particle's acceleration, velocity, and position are updated
        void updateParticle(const std::array<double, 2> force, const double dt);      

        // Simulation calculates the force between particles since it knows where each particle is, but acceleration is calculated by Particle itself
        void updateAcceleration(const std::array<double, 2> force);

        // Acceleration -> velocity -> position is the chain of calculation.
        void updateVelocity(const std::array<double, 2> acceleration, const double dt);

        void updatePosition(const std::array<double, 2> velocity, const double dt);
    };