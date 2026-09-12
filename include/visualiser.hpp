#pragma one
#include <simulation.hpp>
#include <SFML/Graphics.hpp>

class Visualiser{
    private:
        const Simulation& simulation;
        const int framerateLimit;
        sf::RenderWindow window;

    public:
        Visualiser(const Simulation& simulation, const int framerateLimit);

        void drawFrame();

        sf::RenderWindow& returnWindow();
};