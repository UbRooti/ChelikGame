#pragma once

class SoundsManager
{
public:
    static bool playSound(const std::string& soundName)
    {
        auto lSound = ResourceManager::getSound(soundName);

        if (lSound == nullptr)
        {
            return false;
        }

        if (lSound->Playing == sf::SoundSource::Status::Playing)
        {
            lSound->stop();
        }
        lSound->play();

        return true;
    }
};