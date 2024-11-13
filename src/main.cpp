#include "Globals.h"

namespace md // md = Main Data
{
    std::shared_ptr<sf::RenderWindow> window = nullptr;
    std::shared_ptr<FontRenderer> fontRenderer = nullptr;
    std::shared_ptr<Player> player = nullptr;
    std::shared_ptr<Map> map = nullptr;
}; // namespace md = Main Data

#include "Consts.h"

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ResourceManager.h"
#include "Game/SoundsManager.h"
#include "GUI.h"
#include "Game/Game.h"


int main()
{
    ResourceManager::setResourcesPath(VARS::res_path);

    md::window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1280, 720), "Freaktoria");
    md::window->setFramerateLimit(60);
    
    md::fontRenderer = std::make_shared<FontRenderer>(VARS::res_path);

    auto lBackgroundMusic = ResourceManager::loadMusic("DefaultBackgroundMusic", "background.ogg");
    if (lBackgroundMusic == nullptr)
    {
        std::cout << "Failed to load 'DefaultBackgroundMusic'\n";
        md::window->close();
    }

    lBackgroundMusic->setLoop(true);
    lBackgroundMusic->setVolume(10.0f);
    lBackgroundMusic->play();

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