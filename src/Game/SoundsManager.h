#pragma once

#include "../ResourceManager.h"
#include <iostream>

class SoundsManager
{
public:;
    static bool playSound(const std::string& soundName, float volume = 100.f);
};