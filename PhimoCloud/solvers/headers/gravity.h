#pragma once
#ifndef GRAVITY_H
#define GRAVITY_H

#include "bespokeConstructs.h"
#include "constants.h"
#include <vector>

inline void applyGravity(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 a = (phyConsts.G * particles[i].mass * particles[j].mass) * invSquare;

    particles[i].a += a / particles[i].mass;
    particles[j].a -= a / particles[j].mass;
}
inline void applyUniformGravity(std::vector<Particle>& particles, int i, Constants phyConsts) {
    particles[i].a += phyConsts.g;
}

#endif