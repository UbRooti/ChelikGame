#include "SoundsManager.h"

bool SoundsManager::playSound(const std::string& soundName, float volume)
{
    auto lSound = ResourceManager::getSound(soundName);

    if (lSound == nullptr)
    {
        std::cout << "SoundsManager::playSound -> Failed to play sound '" << soundName << "'\n";
        return false;
    }

    if (lSound->getStatus() == sf::SoundSource::Playing)
    {
        lSound->stop();
    }
    lSound->setVolume(volume);

    lSound->play();

    return true;
}