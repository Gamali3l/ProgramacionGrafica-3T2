// File: ProximitySoundSystem.cpp
#include "ProximitySoundSystem.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

using Clock = std::chrono::steady_clock;

ProximitySoundSystem::ProximitySoundSystem() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device." << std::endl;
        return;
    }
    context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
}

ProximitySoundSystem::~ProximitySoundSystem() {
    for (auto& trig : triggers) {
        alDeleteSources(1, &trig.source);
        alDeleteBuffers(1, &trig.buffer);
    }
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void ProximitySoundSystem::addTrigger(const glm::vec3& pos, float radius, const std::string& wavPath) {
    ALuint buffer = loadWav(wavPath);
    if (!buffer) return;

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    triggers.push_back({ pos, radius, buffer, source, false, Clock::now() - std::chrono::seconds(10) });
}

void ProximitySoundSystem::update(const glm::vec3& cameraPos) {
    auto now = Clock::now();
    for (auto& trig : triggers) {
        float dist = glm::distance(cameraPos, trig.position);
        if (dist < trig.radius && !trig.playing && now - trig.lastPlayed >= std::chrono::seconds(10)) {
            alSourcePlay(trig.source);
            trig.playing = true;
            trig.lastPlayed = now;
        }

        ALint state;
        alGetSourcei(trig.source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            trig.playing = false;
        }
    }
}

ALuint ProximitySoundSystem::loadWav(const std::string& filename) {
    drwav wav;
    if (!drwav_init_file(&wav, filename.c_str(), nullptr)) {
        std::cerr << "Failed to load WAV file: " << filename << std::endl;
        return 0;
    }
    std::vector<int16_t> samples(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, samples.data());
    drwav_uninit(&wav);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, wav.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        samples.data(), samples.size() * sizeof(int16_t), wav.sampleRate);

    return buffer;
}
