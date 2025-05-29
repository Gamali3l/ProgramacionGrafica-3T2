#ifndef SOUND_TRIGGER_H
#define SOUND_TRIGGER_H

#include <SFML/Audio.hpp>
#include <glm/glm.hpp>

class SoundTrigger {
public:
    SoundTrigger(const std::string& soundFile, glm::vec3 position, float radius);
    ~SoundTrigger();  
    void update(const glm::vec3& cameraPos);

private:
    sf::SoundBuffer buffer;
    sf::Sound* sound;  
    glm::vec3 position;
    float triggerRadius;
    bool soundPlayed;
};

#endif
