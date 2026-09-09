#include <array>

class Particle{
    private:
        double mass;
        std::array<double, 2> position;       // stores x, y components
        std::array<double, 2> velocity;       // stores x, y components
        std::array<double, 2> acceleration;       // stores x, y components

    public:
        Particle(double mass, std::array<double, 2> position, std::array<double, 2> velocity);      // constructor for single particle

        void updateParticle(const std::array<double, 2> force, const double dt);        // takes the time since last update and recalculates values

        void updateAcceleration(const std::array<double, 2> force);

        void updateVelocity(const std::array<double, 2> acceleration, const double dt);

        void updatePosition(const std::array<double, 2> velocity, const double dt);
    };