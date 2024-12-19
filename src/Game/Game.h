#pragma once

#include "../Globals.h"
#include "../Consts.h"

#include <SFML/Graphics.hpp>
#include <deque>
#include "../JSON.hpp"

#include "../ResourceManager.h"
#include "../Game/SoundsManager.h"
#include "../GUI.h"

class DebugScreenData
{
    float fps;
    std::pair<int, int> render_chunks;

    int playerPosX, playerPosY;
public:
    DebugScreenData();

    void setFPS(float fps);

    float get_fps();

    void setRenderChunks(int viewed_chunks, int chunks);

    std::pair<int, int> getRenderChunks();

    void setPlayerPosition(int x, int y);

    int getPlayerPosX();

    int getPlayerPosY();
};

extern DebugScreenData debugScreenData;

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

const char* get_tex_path(const TEXTURE_ID& id);

class TextureManager
{
    std::map<TEXTURE_ID, sf::Texture*> resources;
public:

    TextureManager(const std::string& path);
    
    ~TextureManager();

    sf::Texture* get(const TEXTURE_ID& id);

    bool contains(const TEXTURE_ID& id);
private:
    void init_textures(const std::string& path);

};

const sf::Color get_color(BLOCK_ID id);

struct Block
{
    sf::RectangleShape rect;
    BLOCK_ID id;
    bool correct;

    Block();

    Block(const sf::RectangleShape& _rect, BLOCK_ID id, const sf::Vector2f& pos);

    void render(sf::RenderWindow& window);

    operator bool();

    static nlohmann::json serialize(const Block& obj);

    static Block deserialize(const nlohmann::json& json);
};

class Chunk
{
    int pos_x;
    int pos_y;

    std::vector<Block*> objects;
public:
    Chunk();

    Chunk(int posX, int posY);

    ~Chunk();

    bool is_in_chunk(int x, int y);

    bool is_in_chunk(const sf::Vector2f& pos);

    Block* get_block(const sf::Vector2f& pos);

    bool remove_block(const sf::Vector2f& pos);

    bool have_blocks();

    bool exists(const sf::Vector2f& pos);

    bool add_block(const sf::Vector2f& pos, BLOCK_ID id);

    bool add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size);

    bool interects_with_chunk(const sf::FloatRect& float_rect);

    bool interects_with_block(const sf::FloatRect& float_rect);

    static nlohmann::json serialize(const Chunk& obj);

    static Chunk deserialize(const nlohmann::json& json);

    void render(sf::RenderWindow& window);
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
    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> mChunks;
    std::vector<Chunk*> mViewChunks;
    sf::RectangleShape mChunkBG;
    const std::string mSaveFile;
public:
    Map(const std::string& file = "res/maps/default.json");

    ~Map();

    void add_block(const sf::Vector2f& pos, BLOCK_ID id);

    void add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size);

    Block* get_block(const sf::Vector2f& pos);

    bool interects(const sf::FloatRect& float_rect) const;

    bool remove_block(const sf::Vector2f& pos);

    void render(sf::RenderWindow& window);

    const std::vector<Chunk*>& get_last_viewed_chunks() const;

private:
    std::pair<int, int> get_chunk_pos(const sf::Vector2f& pos) const;

    std::vector<Chunk*> get_chunks_in_view(sf::RenderWindow& window);
    
    void renderVisibleChunks(sf::RenderWindow& window);
};

double getDistanceBetween(const sf::FloatRect& rect1, const sf::FloatRect& rect2);

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

    sf::RectangleShape mRect;
    sf::RectangleShape mImage;
    sf::Clock mClock;
    BLOCK_ID mCurrentBlock;
    DirectionX mDirectionX;
    DirectionY mDirectionY;
    sf::Vector2f mImgOffset;
    float mReachDistance = VARS::reach_distance;
    sf::Clock mLastBlockUse;

public:
    Player();

    void updateCollision(Map& map);

    bool interects(const sf::FloatRect& rect);

    void render(sf::RenderWindow& window);

    sf::Vector2f get_center() const;

    void useBlock(const sf::Mouse::Button& mouseButton);

    void event(sf::Event& event, sf::RenderWindow& window, Map& map);
};

void draw_debug_screen(sf::RenderWindow& window);

sf::Vector2f getViewSize(sf::RenderWindow& window);

class Game
{

    std::shared_ptr<Map> map;
    std::shared_ptr<Player> player;
    
    std::deque<float> fps_data;

    sf::Clock clock;
    float dt;

public:

    Game();

    ~Game();

    bool loadResources();

    void renderOverlay();

    void renderGame();

    void preRender();

    void proccessEvent(sf::Event& event);

    sf::Vector2f getRenderCenter();
};
