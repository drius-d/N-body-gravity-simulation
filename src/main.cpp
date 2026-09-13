#include <particle.hpp>
#include <simulation.hpp>
#include <visualiser.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <limits>
#include <optional>


int main(){
    int n;
    double dt = 0.001;

    std::cout << "Number of particles: ";
    std::cin >> n;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Simulation simulation(n, dt);
    Visualiser visualiser(simulation, 60);
    sf::RenderWindow& window = visualiser.returnWindow();

    auto previousTime = std::chrono::steady_clock::now();

    double accumulator = 0.0;

    while (window.isOpen()){
        window.clear();

        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)){
                window.close();
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;
        accumulator += elapsedTime.count();

        while (accumulator >= dt){
            simulation.updateSimulation();
            accumulator -= dt;
        }

        double alpha = accumulator / dt;
        visualiser.drawFrame(alpha);
        window.display();
    }
}


