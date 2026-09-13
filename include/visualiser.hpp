#pragma one
#include <simulation.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

class Visualiser{
    private:
        const Simulation& simulation;
        const int framerateLimit;
        sf::RenderWindow window;

    public:
        Visualiser(const Simulation& simulation, const int framerateLimit);

        void drawFrame(const double alpha);

        sf::RenderWindow& returnWindow();
};