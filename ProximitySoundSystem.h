// File: ProximitySoundSystem.h
#ifndef PROXIMITY_SOUND_SYSTEM_H
#define PROXIMITY_SOUND_SYSTEM_H

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <chrono>

struct ProximityTrigger {
    glm::vec3 position;
    float radius;
    ALuint buffer;
    ALuint source;
    bool playing;
    std::chrono::steady_clock::time_point lastPlayed;
};

class ProximitySoundSystem {
public:
    ProximitySoundSystem();
    ~ProximitySoundSystem();

    void addTrigger(const glm::vec3& pos, float radius, const std::string& wavPath);
    void update(const glm::vec3& cameraPos);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ProximityTrigger> triggers;

    ALuint loadWav(const std::string& filename);
};

#endif
