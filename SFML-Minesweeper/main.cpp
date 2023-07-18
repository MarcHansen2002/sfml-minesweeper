#include <iostream>
#include <SFML/Graphics.hpp>
#include "game.h"

int main()
{
    //Initialise window
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Marcsweeper!");
    
    srand(time(NULL));
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
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    gameInst.debugMode = !(gameInst.debugMode);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                MouseClick(&gameInst, &window);
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

void MouseClick(game* gameInst, sf::RenderWindow* window)
{
    //Check if mouse is colliding with an actor
    actor* hovered = NULL;
    bool found = false;
    for (int i = 0; i < gameInst->actors.size(); i++)
    {
        if (IsMouseColliding(*gameInst->actors[i], *window))
        {
            hovered = gameInst->actors[i];
            found = true;
            i = gameInst->actors.size() + 1;
        }
    }
    //If actor is under mouse call left or right click event depending on click
    if (found)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            hovered->OnLeftClick();
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            hovered->OnRightClick();
        }
    }
}

//TODO
//Middle mouse button