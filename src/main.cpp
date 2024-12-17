#include "Globals.h"
#include "Consts.h"

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ResourceManager.h"
#include "Game/SoundsManager.h"
#include "GUI.h"
#include "Game/Game.h"


// test commit add comment


int main()
{
    ResourceManager::setResourcesPath(VARS::res_path);

    md::window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1280, 720), "ChelikGame");
    md::window->setFramerateLimit(60);
    
    md::fontRenderer = std::make_shared<FontRenderer>(VARS::res_path);

    Game gm;

    while (md::window->isOpen())
    {
        sf::Event event;
        while (md::window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed) md::window->close();
            else gm.proccessEvent(event);
        }

        gm.preRender();
        
        md::window->setView(sf::View(gm.getRenderCenter(), getViewSize(*md::window)));
        
        gm.renderGame();

        md::window->setView(sf::View(sf::FloatRect(0, 0, md::window->getSize().x, md::window->getSize().y)));

        gm.renderOverlay();

        md::window->setView(sf::View(gm.getRenderCenter(), getViewSize(*md::window)));

        md::window->display();
    }

    ResourceManager::unloadAllResources();

    return 0;
}