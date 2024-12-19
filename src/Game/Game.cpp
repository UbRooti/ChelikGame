#include "Game.h"

DebugScreenData::DebugScreenData()
: fps(0), render_chunks(0, 0), playerPosX(0), playerPosY(0)
{}

void DebugScreenData::setFPS(float fps)
{
    this->fps = fps;
}

float DebugScreenData::get_fps()
{
    return fps;
}

void DebugScreenData::setRenderChunks(int viewed_chunks, int chunks)
{
    this->render_chunks.first = viewed_chunks;
    this->render_chunks.second = chunks;
}

std::pair<int, int> DebugScreenData::getRenderChunks()
{
    return render_chunks;
}

void DebugScreenData::setPlayerPosition(int x, int y)
{
    this->playerPosX = x;
    this->playerPosY = y;
}

int DebugScreenData::getPlayerPosX()
{
    return this->playerPosX;
}

int DebugScreenData::getPlayerPosY()
{
    return this->playerPosY;
}

DebugScreenData debugScreenData;

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

const sf::Color get_color(BLOCK_ID id)
{
    if (id == BLOCK_ID::AIR) return sf::Color(255, 255, 255, 0);
    else if (id == BLOCK_ID::DIRT_BLOCK) return sf::Color(122, 38, 38, 255);
    else if (id == BLOCK_ID::GRASS_BLOCK) return sf::Color(0, 255, 0, 255);
    else if (id == BLOCK_ID::STONE_BLOCK) return sf::Color(50, 50, 50, 255);

    return sf::Color(255, 255, 255, 255);
}

Block::Block()
: correct(false)
{}

Block::Block(const sf::RectangleShape& _rect, BLOCK_ID id, const sf::Vector2f& pos)
: correct(true), rect(_rect), id(id)
{
    rect.setPosition(pos.x, pos.y);
    rect.setFillColor(get_color(id));
}

void Block::render(sf::RenderWindow& window)
{
    window.draw(rect);
}

Block::operator bool()
{
    return this->correct;
}

nlohmann::json Block::serialize(const Block& obj)
{
    nlohmann::json json;

    json["id"] = obj.id;
    json["rect"]["posX"] = obj.rect.getPosition().x;
    json["rect"]["posY"] = obj.rect.getPosition().y;
    json["rect"]["sizeX"] = obj.rect.getSize().x;
    json["rect"]["sizeY"] = obj.rect.getSize().y;
    json["rect"]["color"] = obj.rect.getFillColor().toInteger();

    return json;
}

Block Block::deserialize(const nlohmann::json& json)
{
    Block obj;

    obj.id = json["id"];
    obj.rect = sf::RectangleShape();

    obj.rect.setPosition(sf::Vector2f(json["rect"]["posX"], json["rect"]["posY"]));
    obj.rect.setSize(sf::Vector2f(json["rect"]["sizeX"], json["rect", "sizeY"]));

    obj.correct = true;
    obj.rect.setFillColor(sf::Color(json["rect"]["color"]));

    return obj;
}

Chunk::Chunk()
: pos_x(0), pos_y(0), objects()
{}

Chunk::Chunk(int posX, int posY)
: pos_x(posX), pos_y(posY), objects()
{}

Chunk::~Chunk()
{
    for (Block* obj : objects)
    {
        delete obj;
    }
}

bool Chunk::is_in_chunk(int x, int y)
{
    int rX = x - (x % VARS::chunk_size);
    int rY = y - (y % VARS::chunk_size);

    return (rX == pos_x && rY == pos_y);
}

bool Chunk::is_in_chunk(const sf::Vector2f& pos)
{
    return is_in_chunk(static_cast<int>(pos.x), static_cast<int>(pos.y));
}

Block* Chunk::get_block(const sf::Vector2f& pos)
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

bool Chunk::remove_block(const sf::Vector2f& pos)
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

bool Chunk::have_blocks()
{
    return !objects.empty();
}

bool Chunk::exists(const sf::Vector2f& pos)
{
    for (Block* obj : objects)
    {
        if (obj->rect.getPosition() == pos) return true;
    }
    return false;
}

bool Chunk::add_block(const sf::Vector2f& pos, BLOCK_ID id)
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

bool Chunk::add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size)
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

bool Chunk::interects_with_chunk(const sf::FloatRect& float_rect)
{
    return float_rect.intersects(sf::FloatRect(sf::Vector2f(pos_x, pos_y), sf::Vector2f(VARS::chunk_size, VARS::chunk_size)));
}

bool Chunk::interects_with_block(const sf::FloatRect& float_rect)
{
    if (!interects_with_chunk(float_rect)) return false;

    for (Block* obj : objects)
    {
        if (obj->rect.getGlobalBounds().intersects(float_rect)) return true;
    }

    return false;
}

nlohmann::json Chunk::serialize(const Chunk& obj)
{
    nlohmann::json json;



    return json;
}

Chunk Chunk::deserialize(const nlohmann::json& json)
{
    Chunk obj;



    return obj;
}

void Chunk::render(sf::RenderWindow& window)
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

Map::Map(const std::string& file)
: mSaveFile(file), mChunks(), mChunkBG(sf::Vector2f(VARS::chunk_size, VARS::chunk_size))
{
    mChunkBG.setFillColor(sf::Color::Green);
    mChunkBG.setTexture(ResourceManager::getTexture("BackgroundGrass").get());
}

Map::~Map()
{
    mChunks.clear();
}

void Map::add_block(const sf::Vector2f& pos, BLOCK_ID id)
{
    auto chunkPos = get_chunk_pos(pos);
    Chunk& chunk = mChunks[chunkPos];

    if (chunk.add_block(pos, id)) return;

    chunk = Chunk(chunkPos.first, chunkPos.second);
    chunk.add_block(pos, id);
}

void Map::add_block(const sf::Vector2f& pos, BLOCK_ID id, const sf::Vector2f& size)
{
    auto chunkPos = get_chunk_pos(pos);
    Chunk& chunk = mChunks[chunkPos];

    if (chunk.add_block(pos, id)) return;

    chunk = Chunk(chunkPos.first, chunkPos.second);
    chunk.add_block(pos, id, size);
}

Block* Map::get_block(const sf::Vector2f& pos)
{
    auto chunkPos = get_chunk_pos(pos);
    auto it = mChunks.find(chunkPos);

    if (it != mChunks.end())
    {
        return it->second.get_block(pos);
    }
    return nullptr;
}

bool Map::interects(const sf::FloatRect& float_rect) const
{
    for (Chunk* obj : mViewChunks)
    {
        if (obj->interects_with_block(float_rect)) return true;
    }
    return false;
}

bool Map::remove_block(const sf::Vector2f& pos)
{
    auto chunkPos = get_chunk_pos(pos);
    auto it = mChunks.find(chunkPos);

    if (it != mChunks.end())
    {
        if (it->second.remove_block(pos))
        {
            if (!it->second.have_blocks())
            {
                mChunks.erase(it);
            }
            return true;
        }
    }

    return false;
}

void Map::render(sf::RenderWindow& window)
{
    renderVisibleChunks(window);
    debugScreenData.setRenderChunks(mViewChunks.size(), mChunks.size());
}

const std::vector<Chunk*>& Map::get_last_viewed_chunks() const
{
    return mViewChunks;
}

std::pair<int, int> Map::get_chunk_pos(const sf::Vector2f& pos) const
{
    int chunkX = static_cast<int>(pos.x) / VARS::chunk_size * VARS::chunk_size;
    int chunkY = static_cast<int>(pos.y) / VARS::chunk_size * VARS::chunk_size;
    return {chunkX, chunkY};
}

std::vector<Chunk*> Map::get_chunks_in_view(sf::RenderWindow& window)
{
    sf::View view = window.getView();
    sf::FloatRect float_rect(view.getCenter() - view.getSize() / 2.f, view.getSize());

    std::vector<Chunk*> vChunks;

    for (auto& [key, chunk] : mChunks)
    {
        if (chunk.interects_with_chunk(float_rect))
        {
            vChunks.push_back(&chunk);
        }
    }

    return vChunks;
}

void Map::renderVisibleChunks(sf::RenderWindow& window)
{

    mViewChunks.clear();

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
            auto it = mChunks.find({x, y});

            mChunkBG.setPosition(x, y);
            window.draw(mChunkBG);

            if (it != mChunks.end())
            {
                mViewChunks.push_back(&it->second);
                it->second.render(window);
            }
        }
    }
}

double getDistanceBetween(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
{
    double posX1 = rect1.left + (rect1.width / 2);
    double posY1 = rect1.top + (rect1.height / 2);

    double posX2 = rect2.left + (rect2.width / 2);
    double posY2 = rect2.top + (rect2.height / 2);

    return std::sqrt((posX2 - posX1) * (posX2 - posX1) + (posY2 - posY1) * (posY2 - posY1));
}

Player::Player()
: mRect(sf::Vector2f(VARS::player_size_x, VARS::player_size_y)), mImage(sf::Vector2f(VARS::player_size_x, VARS::player_size_y * 1.5f)), mImgOffset(0, 0)
{
    mRect.setFillColor(sf::Color(255, 0, 255, 255));
    mRect.setPosition(VARS::max_size_x / 2, VARS::max_size_y / 2);

    mImage.setFillColor(sf::Color::White);
    mImage.setPosition(VARS::max_size_x / 2, VARS::max_size_y / 2);
    mImage.setTexture(ResourceManager::getTexture("Player").get());

    mClock.restart();
    mLastBlockUse.restart();

    mCurrentBlock = BLOCK_ID::DIRT_BLOCK;
}

void Player::updateCollision(Map& map)
{
    if (mLastBlockUse.getElapsedTime().asSeconds() > 0.1)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            useBlock(sf::Mouse::Right);
            mLastBlockUse.restart();
        }
    }

    float dt = mClock.restart().asSeconds();

    if (mRect.getPosition().x < 0)
    {
        mRect.setPosition(0, mRect.getPosition().y);
    }

    if (mRect.getPosition().x + mRect.getSize().x > VARS::max_size_x)
    {
        mRect.setPosition(VARS::max_size_x - mRect.getSize().x, mRect.getPosition().y);
    }

    if (mRect.getPosition().y < 0)
    {
        mRect.setPosition(mRect.getPosition().x, 0);
    }

    if (mRect.getPosition().y + mRect.getSize().y > VARS::max_size_y)
    {
        mRect.setPosition(mRect.getPosition().x, VARS::max_size_y - mRect.getSize().y);
    }


    bool moving = false;
    sf::Vector2f oldPos = mRect.getPosition();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        mRect.setPosition(mRect.getPosition() + sf::Vector2f(0, -VARS::player_speed * dt));
        moving = true;
        mDirectionY = DirectionY::UP;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        mRect.setPosition(mRect.getPosition() + sf::Vector2f(0, VARS::player_speed * dt));
        moving = true;
        mDirectionY = DirectionY::DOWN;
    }

    if (map.interects(mRect.getGlobalBounds()))
    {
        mRect.setPosition(oldPos);
        moving = false;
        mDirectionY = DirectionY::NONE;
    }

    oldPos = mRect.getPosition();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        mRect.setPosition(mRect.getPosition() + sf::Vector2f(-VARS::player_speed * dt, 0));
        moving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        mRect.setPosition(mRect.getPosition() + sf::Vector2f(VARS::player_speed * dt, 0));
        moving = true;
    }

    if (map.interects(mRect.getGlobalBounds()))
    {
        mRect.setPosition(oldPos);
        moving = false;
    }

    if (moving)
    {
        if (mDirectionX == DirectionX::LEFT)
        {
            mImage.setRotation(-2);
            mImgOffset.x = -2;
        }
        else
        {
            mImage.setRotation(2);
            mImgOffset.x = 2;
        }

        if (mDirectionY == DirectionY::UP)
        {
            mImgOffset.y = 2;
        }
        else
        {
            mImgOffset.y = -2;
        }
    }
    else
    {
        mImage.setRotation(0);
        mImgOffset.y = 0;
    }

    debugScreenData.setPlayerPosition(mRect.getPosition().x, mRect.getPosition().y);
}

bool Player::interects(const sf::FloatRect& rect)
{
    return this->mRect.getGlobalBounds().intersects(rect);
}

void Player::render(sf::RenderWindow& window)
{
    mImage.setPosition(mRect.getPosition().x + mImgOffset.x, mRect.getPosition().y - (VARS::player_size_y * 0.5f) + mImgOffset.y);
    window.draw(mImage);



    if (CVARS::debug)
    {
        sf::RectangleShape shape;

        int bWidth = VARS::chunk_border_width;

        shape.setSize(sf::Vector2f(mRect.getSize().x, bWidth));
        shape.setPosition(mRect.getPosition().x, mRect.getPosition().y);
        shape.setFillColor(sf::Color::Red);

        window.draw(shape);

        shape.setPosition(mRect.getPosition().x, mRect.getPosition().y + mRect.getSize().y - bWidth);

        window.draw(shape);

        shape.setPosition(mRect.getPosition().x, mRect.getPosition().y);
        shape.setSize(sf::Vector2f(bWidth, mRect.getSize().y));

        window.draw(shape);

        shape.setPosition(mRect.getPosition().x + mRect.getSize().x - bWidth, mRect.getPosition().y);

        window.draw(shape);
    }
}

sf::Vector2f Player::get_center() const
{
    return mRect.getPosition() + sf::Vector2f(mRect.getSize().x / 2, mRect.getSize().y / 2);
}

void Player::useBlock(const sf::Mouse::Button& mouseButton)
{

    if (md::map.get() == nullptr)
    {
        std::cout << "Player::useBlock -> md::map = nullptr\n";
        return;
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(*md::window);
    sf::Vector2f worldPos = md::window->mapPixelToCoords(pixelPos);

    int posX = worldPos.x - (static_cast<int>(worldPos.x) % VARS::block_size_x);
    int posY = worldPos.y - (static_cast<int>(worldPos.y) % VARS::block_size_y);

    if (mouseButton == sf::Mouse::Right)
    {
        if (
            !interects(sf::FloatRect(posX, posY, VARS::block_size_x, VARS::block_size_y)) &&
            ( getDistanceBetween(sf::FloatRect(posX, posY, VARS::block_size_x, VARS::block_size_y), this->mRect.getGlobalBounds()) <= VARS::reach_distance )
        )
        {
            md::map->add_block(sf::Vector2f(posX, posY), mCurrentBlock);
            mLastBlockUse.restart();
        }
    }
    else if (mouseButton == sf::Mouse::Left)
    {
        md::map->remove_block(sf::Vector2f(posX, posY));
        mLastBlockUse.restart();
    }
}

void Player::event(sf::Event& event, sf::RenderWindow& window, Map& map)
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
                ( getDistanceBetween(sf::FloatRect(posX, posY, VARS::block_size_x, VARS::block_size_y), this->mRect.getGlobalBounds()) <= VARS::reach_distance )
            )
            {
                map.add_block(sf::Vector2f(posX, posY), mCurrentBlock);
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
                mImage.setTextureRect(sf::IntRect(90, 0, -90, 150));
                mDirectionX = DirectionX::LEFT;
                break;
            case sf::Keyboard::D:
                mImage.setTextureRect(sf::IntRect(0, 0, 90, 150));
                mDirectionX = DirectionX::RIGHT;
                break;
            case sf::Keyboard::Num1:
                this->mCurrentBlock = BLOCK_ID::DIRT_BLOCK;
                break;
            case sf::Keyboard::Num2:
                this->mCurrentBlock = BLOCK_ID::GRASS_BLOCK;
                break;
            case sf::Keyboard::Num3:
                this->mCurrentBlock = BLOCK_ID::STONE_BLOCK;
                break;
            default:
                break;
        }
    }
}

void draw_debug_screen(sf::RenderWindow& window)
{
    if (!CVARS::debug) return;
    std::stringstream ss;

    ss << "<<< DEBUG DATA >>>\n";

    ss << "FPS: ";
    ss << debugScreenData.get_fps();
    ss << "\n";

    ss << "Rendering chunks: ";
    ss << debugScreenData.getRenderChunks().first << "/" << debugScreenData.getRenderChunks().second;
    ss << "\n";

    ss << "Player position: ";
    ss << "X (" << debugScreenData.getPlayerPosX() << ") , Y (" << debugScreenData.getPlayerPosY() << ")";
    ss << "\n";

    ss << "Window view type (F3 + W) : ";
    ss << (CVARS::default_view ? "Default" : "Old");
    ss << "\n";

    ss << "<<< DEBUG DATA >>>";

    sf::Vector2f pos = window.mapPixelToCoords(sf::Vector2i(10, 10));
    md::fontRenderer->drawString(window, ss.str(), pos.x, pos.y);
}

sf::Vector2f getViewSize(sf::RenderWindow& window)
{
    if (CVARS::default_view)
    {
        if (window.getSize().x > window.getSize().y)
        {
            int width = VARS::max_view_distance;
            int height = (float(window.getSize().y) / float(window.getSize().x)) * float(VARS::max_view_distance);

            return sf::Vector2f(width, height);
        }
        else if (window.getSize().x < window.getSize().y)
        {
            int width = (float(window.getSize().x) / float(window.getSize().y)) * float(VARS::max_view_distance);
            int height = VARS::max_view_distance;

            return sf::Vector2f(width, height);
        }
        else
        {
            return sf::Vector2f(VARS::max_view_distance, VARS::max_view_distance);
        }
    }
    else
    {
        return (sf::Vector2f) window.getSize();
    }
}

Game::Game()
{
    if (!loadResources())
    {
        std::cout << "Game::Game -> Failed to load resources!\n";
        md::window->close();
    }

    map = std::make_shared<Map>();
    player = std::make_shared<Player>();

    md::map = map;
    md::player = player;
}

Game::~Game()
{
    if (md::player.get() == player.get())
    {
        md::player = nullptr;
    }

    if (md::map.get() == map.get())
    {
        md::map = nullptr;
    }
}

bool Game::loadResources()
{
    auto lBackgroundGrass = ResourceManager::loadTexture("BackgroundGrass", "grass_bg.png");
    if (lBackgroundGrass == nullptr)
    {
        std::cout << "Failed to load background grass texture!\n";
        return false;
    }

    auto lPlayer = ResourceManager::loadTexture("Player", "player.png");
    if (lBackgroundGrass == nullptr)
    {
        std::cout << "Failed to load player texture!\n";
        return false;
    }

    auto lDefaultNotification = ResourceManager::loadSound("DefaultNotification", "button.ogg");
    if (lDefaultNotification == nullptr)
    {
        std::cout << "Failed to load 'DefaultNotification'\n";
        return false;
    }

    auto lBackgroundMusic = ResourceManager::loadMusic("DefaultBackgroundMusic", "background.ogg");
    if (lBackgroundMusic == nullptr)
    {
        std::cout << "Failed to load 'DefaultBackgroundMusic'\n";
        return false;
    }
    else
    {
        lBackgroundMusic->setLoop(true);
        lBackgroundMusic->setVolume(10.0f);
        lBackgroundMusic->play();
    }

    return true;
}

void Game::renderOverlay()
{
    draw_debug_screen(*md::window);
}

void Game::renderGame()
{
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
    debugScreenData.setFPS(fps);

    md::window->clear(sf::Color(0, 150, 0, 255));

    map->render(*md::window);
    player->render(*md::window);
}

void Game::preRender()
{
    player->updateCollision(*map);
}

void Game::proccessEvent(sf::Event& event)
{
    player->event(event, *md::window, *map);
        
    if (event.type == sf::Event::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::F3)
        {
            SoundsManager::playSound("DefaultNotification", 25);
            CVARS::debug = !CVARS::debug;
        }
        else if (event.key.code == sf::Keyboard::W && sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
        {
            CVARS::default_view = !CVARS::default_view;
        }
    }
}

sf::Vector2f Game::getRenderCenter()
{
    return player->get_center();
}