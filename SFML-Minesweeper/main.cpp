#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.h"

int main()
{
    //Initialise window
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Marcsweeper!");
    
    srand(NULL);
    game gameInst;
    gameInst.windowRef = &window;
    gameInst.Init(window);

    sf::Clock clock;
   
    //Game loop
    while (window.isOpen())
    {
        //Events (inputs, closing window etc)
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                std::cout << "button pressed";
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    std::cout << "Debug Toggled";
                    gameInst.DebugMode = !(gameInst.DebugMode);
                    
                }
            }

        }
   

        //Updates & Restarting
        window.clear();

        float elapsed = clock.getElapsedTime().asSeconds(); //Gets time since last frame in ms
        clock.restart(); //Restarts clock for next frame

        gameInst.Update(window, elapsed); //Updates the game scene (collisions, movement etc)
        gameInst.Render(window, elapsed); //Renders all actors onto the screen

        window.display();
    }
   
    //End
    return 0;
}

//TODO
//Sort actors from left to right and only check collisions on actors left of the mouse
//Click events for actors