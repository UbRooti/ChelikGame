#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include <deque>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <utility>
#include "json.hpp"
#include <functional>

namespace VARS
{
    const char* res_path = "resources/";
    const int block_size_x = 30;
    const int block_size_y = 30;
    const int player_size_x = 85; // 85
    const int player_size_y = 85;
    const int player_speed = 500; // 300
    const int max_size_x = 2000 * block_size_x;
    const int max_size_y = 2000 * block_size_y;
    const int chunk_size = 20 * block_size_x;
    const int chunk_border_width = 2;
    const int reach_distance = 500;
} // namespace VARS

namespace CVARS
{
    bool render_chunk_bg = true;
    bool debug = true;
    bool default_view = false;
} // namespace CVARS

class DebugScreenData
{
    float fps;
    std::pair<int, int> render_chunks;

    int playerPosX, playerPosY;
public:
    DebugScreenData()
    : fps(0), render_chunks(0, 0), playerPosX(0), playerPosY(0)
    {}

    void set_fps(float fps)
    {
        this->fps = fps;
    }

    float get_fps()
    {
        return fps;
    }

    void set_render_chunks(int viewed_chunks, int chunks)
    {
        this->render_chunks.first = viewed_chunks;
        this->render_chunks.second = chunks;
    }

    std::pair<int, int> get_render_chunks()
    {
        return render_chunks;
    }

    void setPlayerPosition(int x, int y)
    {
        this->playerPosX = x;
        this->playerPosY = y;
    }

    int getPlayerPosX()
    {
        return this->playerPosX;
    }

    int getPlayerPosY()
    {
        return this->playerPosY;
    }
};
DebugScreenData debugScreenData;

class FontRenderer
{
    sf::Text text;
    sf::Font font;
    int char_size;
public:
    FontRenderer(const std::string& path)
    : char_size(24)
    {
        if (font.loadFromFile(path + "font.ttf"))
        {
            text.setCharacterSize(char_size);
            text.setFillColor(sf::Color::White);
            text.setFont(font);

            if (CVARS::debug)
            {
                std::cout << "Loaded font: " << path + "font.ttf" << "\n";
            }
        }
        else
        {
            if (CVARS::debug)
            {
                std::cout << "Can't find font: " << path + "font.ttf" << "\n";
            }
        }
    }

    void draw_string(sf::RenderWindow& window, const std::string& data, float x, float y, const sf::Color& color = sf::Color::White, int _char_size = 24)
    {
        text.setPosition(x, y);
        if (_char_size != char_size)
        {
            text.setCharacterSize(_char_size);
            char_size = _char_size;
        }
        if (text.getFillColor() != color)
        {
            text.setFillColor(color);
        }
        text.setString(data);

        window.draw(text);
    }

    template<typename T>
    void draw_string(sf::RenderWindow& window, const std::string& data, T x, T y, const sf::Color& color = sf::Color::White, int _char_size = 24)
    {
        draw_string(window, data, static_cast<float>(x), static_cast<float>(y), color, _char_size);
    }
};
FontRenderer fontRenderer(VARS::res_path);

enum BLOCK_ID
{
    INCORRECT = -1,
    AIR = 0,
    GRASS_BLOCK = 1,
    DIRT_BLOCK = 2,
    STONE_BLOCK = 3
};

enum TEXTURE_ID
{
    GRASS_BG,
    PLAYER_IMG
};

const char* get_tex_path(const TEXTURE_ID& id)
{
    switch (id)
    {
    case GRASS_BG:
        return "grass_bg.png";
    case PLAYER_IMG:
        return "player.png";
    default:
        return "";
        break;
    }
}

class TextureManager
{
    std::map<TEXTURE_ID, sf::Texture*> resources;
public:

    TextureManager(const std::string& path)
    {
        init_textures(path);
    }
    
    ~TextureManager()
    {
        for (std::pair<TEXTURE_ID, sf::Texture*> p : resources)
        {
            delete p.second;
        }

        resources.clear();
    }

    sf::Texture* get(const TEXTURE_ID& id)
    {
        for (std::pair<TEXTURE_ID, sf::Texture*> p : resources)
        {
            if (p.first == id)
            {
                return p.second;
            }
        }

        return 0;
    }

    bool contains(const TEXTURE_ID& id)
    {
        return resources.find(id) != resources.end();
    }
private:
    void init_textures(const std::string& path)
    {
        sf::Texture* texture = new sf::Texture;
        std::string tex_path = path + get_tex_path(TEXTURE_ID::GRASS_BG);
        if (texture->loadFromFile(tex_path))
        {
            resources.insert(std::pair<TEXTURE_ID, sf::Texture*> { TEXTURE_ID::GRASS_BG, texture });
            std::cout << "Loaded texture: " << tex_path << "\n";
        }
        else
        {
            std::cerr << "Can't find: " << tex_path << "\n";
        }

        texture = new sf::Texture;
        tex_path = path + get_tex_path(TEXTURE_ID::PLAYER_IMG);
        if (texture->loadFromFile(tex_path))
        {
            resources.insert(std::pair<TEXTURE_ID, sf::Texture*> { TEXTURE_ID::PLAYER_IMG, texture });
            std::cout << "Loaded texture: " << tex_path << "\n";
        }
        else
        {
            std::cerr << "Can't find: " << tex_path << "\n";
        }
    }

};
TextureManager textureManager(VARS::res_path);

class BlockInfo
{
    sf::RectangleShape rect;
    BLOCK_ID block_id;
public:
    virtual const sf::Vector2f& get_size() const
    {
        return this->rect.getSize();
    }

    virtual const sf::Vector2f& get_pos() const
    {
        return this->rect.getPosition();
    }

    virtual BLOCK_ID get_id()
    {
        return this->block_id;
    }
};

const sf::Color get_color(BLOCK_ID id)
{
    if (id == BLOCK_ID::AIR) return sf::Color(255, 255, 255, 0);
    else if (id == BLOCK_ID::DIRT_BLOCK) return sf::Color(122, 38, 38, 255);
    else if (id == BLOCK_ID::GRASS_BLOCK) return sf::Color(0, 255, 0, 255);
    else if (id == BLOCK_ID::STONE_BLOCK) return sf::Color(50, 50, 50, 255);
    
    return sf::Color(255, 255, 255, 255);
}

struct Block
{
    sf::RectangleShape rect;
    BLOCK_ID id;
    bool correct;

    Block()
    : correct(false)
    {}

    Block(const sf::RectangleShape& _rect, BLOCK_ID id, const sf::Vector2f& pos)
    : correct(true), rect(_rect), id(id)
    {
        rect.setPosition(pos.x, pos.y);
        rect.setFillColor(get_color(id));
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(rect);
    }

    operator bool()
    {
        return this->correct;
    }

    static nlohmann::json serialize(const Block& obj)
    {
        nlohmann::json json;

        json["id"] = obj.id;
        json["posX"] = obj.rect.getPosition().x;
        json["posY"] = obj.rect.getPosition().y;
        json["rect"]["x"] = obj.rect.getPosition().x;
        json["rect"]["y"] = obj.rect.getPosition().y;
        json["rect"]["color"] = obj.rect.getFillColor().toInteger();

        return json;
    }

    static Block deserialize(const nlohmann::json& json)
    {
        
    }
};

class Chunk
{
    int pos_x;
    int pos_y;

    std::vector<Block*> objects;
public:
    Chunk()
    : pos_x(0), pos_y(0), objects()
    {}

    Chunk(int posX, int posY)
    : pos_x(posX), pos_y(posY), objects()
    {}

    ~Chunk()
    {
        for (Block* obj : objects)
        {
            delete obj;
        }
    }

    bool is_in_chunk(int x, int y)
    {
        int rX = x - (x % VARS::chunk_size);
        int rY = y - (y % VARS::chunk_size);

        return (rX == pos_x && rY == pos_y);
    }

    bool is_in_chunk(const sf::Vector2f& pos)
    {
        return is_in_chunk(static_cast<int>(pos.x), static_cast<int>(pos.y));
    }

    Block* get_block(const sf::Vector2f& pos)
    {
        if (is_in_chunk(pos))
        {
            for (Block* obj : this->objects)
            {
                if (obj->rect.getPosition() == pos) return obj;
            }
            return nullptr;
        }
        return nullptr;
    }

    bool remove_block(const sf::Vector2f& pos)
    {
        for (auto it = objects.begin(); it != objects.end(); ++it)
        {
            if ((*it)->rect.getPosition() == pos)
            {
                delete *it;
                objects.erase(it);
                return true;
            }
        }
        return false;
    }

    bool have_blocks()
    {
        return !objects.empty();
    }

    bool exists(const sf::Vector2f& pos)
    {
        for (Block* obj : objects)
        {
            if (obj->rect.getPosition() == pos) return true;
        }
        return false;
    }

    bool add_block(const sf::Vector2f& pos, BLOCK_ID id)
    {
        if (is_in_chunk(pos))
        {
            if (exists(pos))
            {
                return true;
            }
            
            Block* obj = new Block(sf::RectangleShape(sf::Vector2f(VARS::block_size_x, VARS::block_size_y)), id, pos);
            objects.push_back(obj);
            return true;
        }
        return false;
    }

    bool add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size)
    {
        if (is_in_chunk(pos))
        {
            if (exists(pos))
            {
                return true;
            }
            
            Block* obj = new Block(sf::RectangleShape(size), id, pos);
            objects.push_back(obj);
            return true;
        }
        return false;
    }

    bool interects_with_chunk(const sf::FloatRect& float_rect)
    {
        return float_rect.intersects(sf::FloatRect(sf::Vector2f(pos_x, pos_y), sf::Vector2f(VARS::chunk_size, VARS::chunk_size)));
    }

    bool interects_with_block(const sf::FloatRect& float_rect)
    {
        if (!interects_with_chunk(float_rect)) return false;

        for (Block* obj : objects)
        {
            if (obj->rect.getGlobalBounds().intersects(float_rect)) return true;
        }

        return false;
    }

    void render(sf::RenderWindow& window)
    {

        for (Block* obj : objects)
        {
            obj->render(window);
        }

        if (CVARS::debug)
        {
            sf::RectangleShape shape;

            shape.setSize(sf::Vector2f(VARS::chunk_size, VARS::chunk_border_width));
            shape.setPosition(pos_x, pos_y);
            shape.setFillColor(sf::Color::White);

            window.draw(shape);
            
            shape.setPosition(pos_x, pos_y + VARS::chunk_size - VARS::chunk_border_width);

            window.draw(shape);

            shape.setPosition(pos_x, pos_y);
            shape.setSize(sf::Vector2f(VARS::chunk_border_width, VARS::chunk_size));

            window.draw(shape);

            shape.setPosition(pos_x + VARS::chunk_size - VARS::chunk_border_width, pos_y);

            window.draw(shape);
        }
    }
};

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const
    {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class Map
{
    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> chunks;
    std::vector<Chunk*> viewChunks;
    sf::RectangleShape chunk_bg;
    const std::string m_saveFile;
public:
    Map(const std::string& file = "res/maps/default.json")
    : m_saveFile(file), chunks(), chunk_bg(sf::Vector2f(VARS::chunk_size, VARS::chunk_size))
    {
        chunk_bg.setFillColor(sf::Color::Green);
        chunk_bg.setTexture(textureManager.get(TEXTURE_ID::GRASS_BG));
    }

    ~Map()
    {
        chunks.clear();
    }

    void add_block(const sf::Vector2f& pos, BLOCK_ID id)
    {
        auto chunkPos = get_chunk_pos(pos);
        Chunk& chunk = chunks[chunkPos];

        if (chunk.add_block(pos, id)) return;

        chunk = Chunk(chunkPos.first, chunkPos.second);
        chunk.add_block(pos, id);
    }

    void add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size)
    {
        auto chunkPos = get_chunk_pos(pos);
        Chunk& chunk = chunks[chunkPos];

        if (chunk.add_block(pos, id)) return;

        chunk = Chunk(chunkPos.first, chunkPos.second);
        chunk.add_block(pos, id, size);
    }

    Block* get_block(const sf::Vector2f& pos)
    {
        auto chunkPos = get_chunk_pos(pos);
        auto it = chunks.find(chunkPos);

        if (it != chunks.end())
        {
            return it->second.get_block(pos);
        }
        return nullptr;
    }

    bool interects(const sf::FloatRect& float_rect) const
    {
        for (Chunk* obj : viewChunks)
        {
            if (obj->interects_with_block(float_rect)) return true;
        }
        return false;
    }

    bool remove_block(const sf::Vector2f& pos)
    {
        auto chunkPos = get_chunk_pos(pos);
        auto it = chunks.find(chunkPos);

        if (it != chunks.end())
        {
            if (it->second.remove_block(pos))
            {
                if (!it->second.have_blocks())
                {
                    chunks.erase(it);
                }
                return true;
            }
        }

        return false;
    }

    void render(sf::RenderWindow& window)
    {
        renderVisibleChunks(window);
        debugScreenData.set_render_chunks(viewChunks.size(), chunks.size());
    }

    const std::vector<Chunk*>& get_last_viewed_chunks() const
    {
        return viewChunks;
    }

private:
    std::pair<int, int> get_chunk_pos(const sf::Vector2f& pos) const
    {
        int chunkX = static_cast<int>(pos.x) / VARS::chunk_size * VARS::chunk_size;
        int chunkY = static_cast<int>(pos.y) / VARS::chunk_size * VARS::chunk_size;
        return {chunkX, chunkY};
    }

    std::vector<Chunk*> get_chunks_in_view(sf::RenderWindow& window)
    {
        sf::View view = window.getView();
        sf::FloatRect float_rect(view.getCenter() - view.getSize() / 2.f, view.getSize());

        std::vector<Chunk*> vChunks;

        for (auto& [key, chunk] : chunks)
        {
            if (chunk.interects_with_chunk(float_rect))
            {
                vChunks.push_back(&chunk);
            }
        }

        return vChunks;
    }
    
    void renderVisibleChunks(sf::RenderWindow& window)
    {

        viewChunks.clear();

        sf::View view = window.getView();

        sf::FloatRect visibleArea(view.getCenter() - view.getSize() / 2.f, view.getSize());

        int minChunkX = static_cast<int>(visibleArea.left) - (static_cast<int>(visibleArea.left) % VARS::chunk_size);
        int maxChunkX = static_cast<int>(visibleArea.left + visibleArea.width) - (static_cast<int>(visibleArea.left + visibleArea.width) % VARS::chunk_size);
        int minChunkY = static_cast<int>(visibleArea.top) - (static_cast<int>(visibleArea.top) % VARS::chunk_size);
        int maxChunkY = static_cast<int>(visibleArea.top + visibleArea.height) - (static_cast<int>(visibleArea.top + visibleArea.height) % VARS::chunk_size);
        
        for (int x = minChunkX; x <= maxChunkX; x += VARS::chunk_size)
        {
            for (int y = minChunkY; y <= maxChunkY; y += VARS::chunk_size)
            {
                auto it = chunks.find({x, y});

                chunk_bg.setPosition(x, y);
                window.draw(chunk_bg);

                if (it != chunks.end())
                {
                    viewChunks.push_back(&it->second);
                    it->second.render(window);
                }
            }
        }
    }

    bool save()
    {

    }

    bool load()
    {

    }
};

double getDistanceBetween(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
{
    double posX1 = rect1.left + (rect1.width / 2);
    double posY1 = rect1.top + (rect1.height / 2);

    double posX2 = rect2.left + (rect2.width / 2);
    double posY2 = rect2.top + (rect2.height / 2);

    return std::sqrt((posX2 - posX1) * (posX2 - posX1) + (posY2 - posY1) * (posY2 - posY1));
}

class Player
{
    enum DirectionX
    {
        LEFT,
        RIGHT
    };

    enum DirectionY
    {
        UP,
        DOWN,
        NONE
    };

private:

    sf::RectangleShape rect;
    sf::RectangleShape image;
    sf::Clock clock;
    BLOCK_ID current_block;
    DirectionX direction_x;
    DirectionY direction_y;
    sf::Vector2f img_offset;

public:
    Player()
    : rect(sf::Vector2f(VARS::player_size_x, VARS::player_size_y)), image(sf::Vector2f(VARS::player_size_x, VARS::player_size_y * 1.5f)), img_offset(0, 0)
    {
        rect.setFillColor(sf::Color(255, 0, 255, 255));
        rect.setPosition(VARS::max_size_x / 2, VARS::max_size_y / 2);

        image.setFillColor(sf::Color::White);
        image.setPosition(VARS::max_size_x / 2, VARS::max_size_y / 2);
        // image.setOrigin(sf::Vector2f(VARS::player_size_x, VARS::player_size_y * 1.5f) / 2.0f);
        image.setTexture(textureManager.get(TEXTURE_ID::PLAYER_IMG));

        clock.restart();
        current_block = BLOCK_ID::DIRT_BLOCK;
    }

    void updateCollision(Map& map)
    {
        float dt = clock.restart().asSeconds();

        if (rect.getPosition().x < 0)
        {
            rect.setPosition(0, rect.getPosition().y);
        }

        if (rect.getPosition().x + rect.getSize().x > VARS::max_size_x)
        {
            rect.setPosition(VARS::max_size_x - rect.getSize().x, rect.getPosition().y);
        }

        if (rect.getPosition().y < 0)
        {
            rect.setPosition(rect.getPosition().x, 0);
        }

        if (rect.getPosition().y + rect.getSize().y > VARS::max_size_y)
        {
            rect.setPosition(rect.getPosition().x, VARS::max_size_y - rect.getSize().y);
        }


        bool moving = false;
        sf::Vector2f oldPos = rect.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            rect.setPosition(rect.getPosition() + sf::Vector2f(0, -VARS::player_speed * dt));
            moving = true;
            direction_y = DirectionY::UP;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            rect.setPosition(rect.getPosition() + sf::Vector2f(0, VARS::player_speed * dt));
            moving = true;
            direction_y = DirectionY::DOWN;
        }

        if (map.interects(rect.getGlobalBounds()))
        {
            rect.setPosition(oldPos);
            moving = false;
            direction_y = DirectionY::NONE;
        }

        oldPos = rect.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            rect.setPosition(rect.getPosition() + sf::Vector2f(-VARS::player_speed * dt, 0));
            moving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            rect.setPosition(rect.getPosition() + sf::Vector2f(VARS::player_speed * dt, 0));
            moving = true;
        }

        if (map.interects(rect.getGlobalBounds()))
        {
            rect.setPosition(oldPos);
            moving = false;
        }

        if (moving)
        {
            if (direction_x == DirectionX::LEFT)
            {
                image.setRotation(-2);
                img_offset.x = -2;
            }
            else
            {
                image.setRotation(2);
                img_offset.x = 2;
            }

            if (direction_y == DirectionY::UP)
            {
                img_offset.y = 2;
            }
            else
            {
                img_offset.y = -2;
            }
        }
        else
        {
            image.setRotation(0);
            img_offset.y = 0;
        }

        debugScreenData.setPlayerPosition(rect.getPosition().x, rect.getPosition().y);
    }

    bool interects(const sf::FloatRect& rect)
    {
        return this->rect.getGlobalBounds().intersects(rect);
    }

    void render(sf::RenderWindow& window)
    {
        // window.draw(rect);
        // image.setPosition(rect.getPosition().x + img_offset.x  + (rect.getSize().x / 2), rect.getPosition().y - (VARS::player_size_y * 0.5f) + img_offset.y + (rect.getSize().y / 2));
        image.setPosition(rect.getPosition().x + img_offset.x, rect.getPosition().y - (VARS::player_size_y * 0.5f) + img_offset.y);
        window.draw(image);



        if (CVARS::debug)
        {
            sf::RectangleShape shape;

            int bWidth = VARS::chunk_border_width;

            shape.setSize(sf::Vector2f(rect.getSize().x, bWidth));
            shape.setPosition(rect.getPosition().x, rect.getPosition().y);
            shape.setFillColor(sf::Color::Red);

            window.draw(shape);
            
            shape.setPosition(rect.getPosition().x, rect.getPosition().y + rect.getSize().y - bWidth);

            window.draw(shape);

            shape.setPosition(rect.getPosition().x, rect.getPosition().y);
            shape.setSize(sf::Vector2f(bWidth, rect.getSize().y));

            window.draw(shape);

            shape.setPosition(rect.getPosition().x + rect.getSize().x - bWidth, rect.getPosition().y);

            window.draw(shape);
        }
    }

    sf::Vector2f get_center() const
    {
        return rect.getPosition() + sf::Vector2f(rect.getSize().x / 2, rect.getSize().y / 2);
    }

    void event(sf::Event& event, sf::RenderWindow& window, Map& map)
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

            int posX = worldPos.x - (static_cast<int>(worldPos.x) % VARS::block_size_x);
            int posY = worldPos.y - (static_cast<int>(worldPos.y) % VARS::block_size_y);
            
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                if (
                    !interects(sf::FloatRect(posX, posY, VARS::block_size_x, VARS::block_size_y)) &&
                    ( getDistanceBetween(sf::FloatRect(posX, posY, VARS::block_size_x, VARS::block_size_y), this->rect.getGlobalBounds()) <= VARS::reach_distance )
                )
                {
                    map.add_block(sf::Vector2f(posX, posY), current_block);
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Left)
            {
                map.remove_block(sf::Vector2f(posX, posY));
            }
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            switch ((sf::Keyboard::Key) event.key.code)
            {
                case sf::Keyboard::A:
                    image.setTextureRect(sf::IntRect(90, 0, -90, 150));
                    direction_x = DirectionX::LEFT;
                    break;
                case sf::Keyboard::D:
                    image.setTextureRect(sf::IntRect(0, 0, 90, 150));
                    direction_x = DirectionX::RIGHT;
                    break;
                case sf::Keyboard::Num1:
                    this->current_block = BLOCK_ID::DIRT_BLOCK;
                    break;
                case sf::Keyboard::Num2:
                    this->current_block = BLOCK_ID::GRASS_BLOCK;
                    break;
                case sf::Keyboard::Num3:
                    this->current_block = BLOCK_ID::STONE_BLOCK;
                    break;
                default:
                    break;
            }
        }
    }
};

void draw_debug_screen(sf::RenderWindow& window)
{
    if (!CVARS::debug) return;
    std::stringstream ss;

    ss << "FPS: ";
    ss << debugScreenData.get_fps();
    ss << "\n";
    
    ss << "Rendering chunks: ";
    ss << debugScreenData.get_render_chunks().first << "/" << debugScreenData.get_render_chunks().second;
    ss << "\n";

    ss << "Player position: ";
    ss << "X (" << debugScreenData.getPlayerPosX() << ") , Y (" << debugScreenData.getPlayerPosY() << ")";
    ss << "\n";

    ss << "Window view type: ";
    ss << (CVARS::default_view ? "Default" : "Constant");
    ss << "\n";

    sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(10, 10));
    fontRenderer.draw_string(window, ss.str(), pos.x, pos.y);
}

int main()
{
    Map map;
    Player player;

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Freaktoria");
    window.setFramerateLimit(60);
    
    std::deque<float> fps_data;

    sf::Clock clock;
    float dt;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                player.event(event, window, map);
            
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::F3)
                {
                    CVARS::debug = !CVARS::debug;
                }
                else if (event.key.code == sf::Keyboard::W && sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
                {
                    CVARS::default_view = !CVARS::default_view;
                }

            }
        }

        player.updateCollision(map);
        window.setView(sf::View(player.get_center(), (sf::Vector2f) window.getSize()));

        dt = clock.restart().asSeconds();

        fps_data.push_front(1.0f / dt);

        if (fps_data.size() > 60)
        {
            fps_data.pop_back();
        }

        double fps = 0;
        for (float i : fps_data)
        {
            fps += i;
        }

        fps = float(int((fps / fps_data.size()) * 100)) / 100.0f;
        debugScreenData.set_fps(fps);

        window.clear(sf::Color(0, 0, 150, 255));

        map.render(window);
        player.render(window);

        draw_debug_screen(window);

        window.display();
    }

    return 0;
}