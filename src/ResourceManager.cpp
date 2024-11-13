#include "ResourceManager.h"

std::string ResourceManager::mResourcesPath;

std::map<const std::string, std::shared_ptr<sf::Texture>> ResourceManager::mTexturesArray;
std::map<const std::string, std::shared_ptr<sf::Music>> ResourceManager::mMusicArray;
std::map<const std::string, std::shared_ptr<sf::Sound>> ResourceManager::mSoundArray;

void ResourceManager::unloadAllResources()
{
    std::cout << "ResourceManager::unloadResources -> Unloading all resources!\n{\n";

    std::cout << "\tBefore clearing: "
              << "Textures: " << mTexturesArray.size() << ", "
              << "Music: " << mMusicArray.size() << ", "
              << "Sounds: " << mSoundArray.size() << std::endl;

    mTexturesArray.clear();
    mMusicArray.clear();
    mSoundArray.clear();

    std::cout << "\tAfter clearing: "
              << "Textures: " << mTexturesArray.size() << ", "
              << "Music: " << mMusicArray.size() << ", "
              << "Sounds: " << mSoundArray.size() << std::endl;

    std::cout << "}\nResourceManager::unloadResources -> Completed unloading resources! Result: " << (mTexturesArray.empty() && mMusicArray.empty() && mSoundArray.empty() ? "Success!" : "Some resources are not unloaded!") << "\n";
}

void ResourceManager::setResourcesPath(const std::string& path)
{
    mResourcesPath = path;
}

std::shared_ptr<sf::Texture> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath)
{
    sf::Texture lTexture;

    if (lTexture.loadFromFile(mResourcesPath + texturePath))
    {
        auto result = mTexturesArray.emplace(
            std::pair<std::string, std::shared_ptr<sf::Texture>>
                ( textureName, std::make_shared<sf::Texture>(lTexture) )
        );

        if (result.second)
        {
            return result.first->second;
        }
    }

    std::cout << "ResourceManager::loadTexture -> Failed to load texture from '" << texturePath << "' with name '" << textureName << "'\n";
    
    return nullptr;
}

std::shared_ptr<sf::Texture> ResourceManager::getTexture(const std::string& textureName)
{
    auto it = mTexturesArray.find(textureName);

    if (it != mTexturesArray.end())
    {
        return it->second;
    }

    std::cout << "ResourceManager::getTexture -> Failed to get texture with name '" << textureName << "'\n";

    return nullptr;
}

std::shared_ptr<sf::Music> ResourceManager::loadMusic(const std::string& musicName, const std::string& musicPath)
{
    std::shared_ptr<sf::Music> lMusic = std::make_shared<sf::Music>();

    if (lMusic->openFromFile(mResourcesPath + musicPath))
    {
        auto result = mMusicArray.emplace(
            std::pair<std::string, std::shared_ptr<sf::Music>>
                ( musicName, std::move(lMusic))
        );

        if (result.second)
        {
            return result.first->second;
        }
    }

    std::cout << "ResourceManager::loadMusic -> Failed to load music from '" << musicPath << "' with name '" << musicName << "'\n";
    
    return nullptr;
}

std::shared_ptr<sf::Music> ResourceManager::getMusic(const std::string& musicName)
{
    auto it = mMusicArray.find(musicName);

    if (it != mMusicArray.end())
    {
        return it->second;
    }

    std::cout << "ResourceManager::getMusic -> Failed to get music with name '" << musicName << "'\n";
    
    return nullptr;
}

std::shared_ptr<sf::Sound> ResourceManager::loadSound(const std::string& soundName, const std::string& soundPath)
{
    sf::Sound lSound;
    sf::SoundBuffer lBuffer;

    if (lBuffer.loadFromFile(mResourcesPath + soundPath))
    {
        lSound = sf::Sound(lBuffer);

        auto result = mSoundArray.emplace(
            std::pair<std::string, std::shared_ptr<sf::Sound>>
                ( soundName, std::make_shared<sf::Sound>(lSound) )
        );

        if (result.second)
        {
            return result.first->second;
        }
    }

    std::cout << "ResourceManager::loadSound -> Failed to load sound from '" << soundPath << "' with name '" << soundName << "'\n";
    
    return nullptr;
}

std::shared_ptr<sf::Sound> ResourceManager::getSound(const std::string& soundName)
{
    auto it = mSoundArray.find(soundName);

    if (it != mSoundArray.end())
    {
        return it->second;
    }
    
    std::cout << "ResourceManager::getSound -> Failed to get sound with name '" << soundName << "'\n";

    return nullptr;
}