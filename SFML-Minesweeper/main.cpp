#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    //Initialise window
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Marcsweeper!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
   
    //Game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
   
        window.clear();
        window.draw(shape);
        window.display();
    }
   
    //End
    return 0;
}