#include <SFML/Graphics.hpp>
#include <optional>

int main(){

    // create window

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "My Program");      // going to need some scaling function that converts position to pixel size of window
    window.setFramerateLimit(60);

    sf::RectangleShape rect;

    sf::Vector2f rectanglePosition(600, 350);       // going to have to convert our position vectors into sf::Vector2f      

    // probably set size proportional to mass so again need a function that takes the mass and spits out some appropriate size?. 
    // mass proportional volume so m = kr^3 so double the mass should mean times (2)^1/3 radius. This assumes particles are same density. 
    // k we can change to how we want

    rect.setPosition(rectanglePosition);  
    rect.setSize(sf::Vector2f (100, 100));    
    
    float xVelocity = 3;
    float yVelocity = 3;

    while (window.isOpen()){

        while (const std::optional event = window.pollEvent()){
            
            // Window closed: exit
            
            if (event->is<sf::Event::Closed>() || 
                (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)){
                window.close(); 
            }
        }

        rectanglePosition.x += xVelocity;
        rectanglePosition.y += yVelocity;

        rect.setPosition(rectanglePosition);

        window.clear();
        window.draw(rect);
        window.display();

        if (rectanglePosition.x + rect.getSize().x >= 1280){
            xVelocity = -xVelocity;
        }
        if (rectanglePosition.x <= 0){
            xVelocity = -xVelocity;
        }
        if (rectanglePosition.y + rect.getSize().y >= 720){
            yVelocity = -yVelocity;
        }
        if (rectanglePosition.y <= 0){
            yVelocity = -yVelocity;
        }
    }   

    

    return 0;
}
