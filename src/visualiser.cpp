#include <visualiser.hpp>
#include <simulation.hpp>
#include <cmath> 



Visualiser::Visualiser(const Simulation& s, const int fLimit)
: simulation(s), framerateLimit(fLimit), window(sf::VideoMode({1000, 1000}), "N-Body Simulation") {}

void Visualiser::drawFrame(){
    
    int xScaler = 10;
    int yScaler = 10;

    for (const std::unique_ptr<Particle>& p : simulation.returnParticles()){
        double particleMass = p->getMass();
        double k = 0.001;
        double radius = std::cbrt((particleMass / k));

        std::array<double, 2> position = p->getPosition(); 
        sf::Vector2f screenPosition;

        screenPosition.x = position[0] * xScaler;
        screenPosition.y = position[1] * yScaler;

        sf::CircleShape circle(radius);
        circle.setPosition(screenPosition);

        window.draw(circle);
    }
}

sf::RenderWindow& Visualiser::returnWindow(){
    return window;
}