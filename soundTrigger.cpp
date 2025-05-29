#include "SoundTrigger.h"
#include <iostream>

SoundTrigger::SoundTrigger(const std::string& soundFile, glm::vec3 position, float radius)
    : position(position), triggerRadius(radius), soundPlayed(false), sound(nullptr)
{
    if (!buffer.loadFromFile(soundFile)) {
        std::cerr << "Failed to load sound file: " << soundFile << std::endl;
        return;
    }

    sound = new sf::Sound();         
    sound->setBuffer(buffer);
}

SoundTrigger::~SoundTrigger()
{
    delete sound;                    
}

void SoundTrigger::update(const glm::vec3& cameraPos)
{
    if (!sound) return;

    float distance = glm::distance(cameraPos, position);

    if (distance < triggerRadius) {
        if (!soundPlayed) {
            sound->play();
            soundPlayed = true;
        }
    }
    else {
        soundPlayed = false;
    }
}
