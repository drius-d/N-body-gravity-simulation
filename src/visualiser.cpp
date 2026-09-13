#include <visualiser.hpp>
#include <simulation.hpp>
#include <cmath> 



Visualiser::Visualiser(const Simulation& s, const int fLimit)
: simulation(s), framerateLimit(fLimit), window(sf::VideoMode({1000, 1000}), "N-Body Simulation") {}

void Visualiser::drawFrame(const double alpha){
    
    const double scale = 10.0;

    for (int i = 0; i < simulation.returnParticles().size(); i++){
        const double particleMass = simulation.returnParticles()[i]->getMass();
        const double radius = simulation.returnParticles()[i]->getRadius();

        const auto& p = simulation.returnParticles()[i];
        const std::array<double, 2>& previousPosition = simulation.returnPreviousPositions()[i];
        const std::array<double, 2>& currentPosition = p->getPosition();

        sf::Vector2f screenPosition;

        screenPosition.x = (previousPosition[0] + alpha * (currentPosition[0] - previousPosition[0])) * scale;
        screenPosition.y = (previousPosition[1] + alpha * (currentPosition[1] - previousPosition[1])) * scale;
        const double screenRadius = radius * scale;

        sf::CircleShape circle(radius * scale);
        circle.setPosition(screenPosition);

        window.draw(circle);

    }
}

sf::RenderWindow& Visualiser::returnWindow(){
    return window;
}