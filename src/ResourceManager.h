#pragma once

#include <map>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class ResourceManager
{
    static std::string mResourcesPath;

    static std::map<const std::string, std::shared_ptr<sf::Texture>> mTexturesArray;
    static std::map<const std::string, std::shared_ptr<sf::Music>> mMusicArray;
    static std::map<const std::string, std::shared_ptr<sf::Sound>> mSoundArray;
public:
    static void unloadAllResources();
    static void setResourcesPath(const std::string& path);

    ~ResourceManager() = delete;
    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    ResourceManager(ResourceManager&&) = delete;

    static std::shared_ptr<sf::Texture> loadTexture(const std::string& textureName, const std::string& texturePath);
    static std::shared_ptr<sf::Texture> getTexture(const std::string& textureName);

    static std::shared_ptr<sf::Music> loadMusic(const std::string& musicName, const std::string& musicPath);
    static std::shared_ptr<sf::Music> getMusic(const std::string& musicName);

    static std::shared_ptr<sf::Sound> loadSound(const std::string& soundName, const std::string& soundPath);
    static std::shared_ptr<sf::Sound> getSound(const std::string& soundName);

};