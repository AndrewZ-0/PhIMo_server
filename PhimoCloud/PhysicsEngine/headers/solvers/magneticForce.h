#pragma once
#ifndef MAGNETICFORCE_H
#define MAGNETICFORCE_H

#include "../bespokeConstructs.h"
#include "../constants.h"
#include <vector>

inline void applyMagneticForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 alpha = phyConsts.km * particles[i].charge * particles[j].charge * invSquare;

    particles[i].a -= cross(particles[i].v, cross(particles[j].v, alpha)) / particles[i].mass;
    particles[j].a += cross(particles[j].v, cross(particles[i].v, alpha)) / particles[j].mass;
}
inline void applyUniformMagneticForce(std::vector<Particle>& particles, int i, Constants phyConsts) {
    particles[i].a += cross(particles[i].v, phyConsts.B) / particles[i].mass;
}

#endif