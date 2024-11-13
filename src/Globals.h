#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

class FontRenderer;
class Player;
class Map;

namespace md // md = Main Data
{
    extern std::shared_ptr<sf::RenderWindow> window;
    extern std::shared_ptr<FontRenderer> fontRenderer;
    extern std::shared_ptr<Player> player;
    extern std::shared_ptr<Map> map;

}; // namespace md = Main Data