#include "Globals.h"

namespace md // md = Main Data
{
    std::shared_ptr<sf::RenderWindow> window = nullptr;
    std::shared_ptr<FontRenderer> fontRenderer = nullptr;
    std::shared_ptr<Player> player = nullptr;
    std::shared_ptr<Map> map = nullptr;
}; // namespace md = Main Data