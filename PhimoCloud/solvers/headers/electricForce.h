#pragma once
#ifndef ELECTRICFORCE_H
#define ELECTRICFORCE_H

#include "bespokeConstructs.h"
#include "constants.h"
#include <vector>

inline void applyElectricForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 a = (phyConsts.ke * particles[i].charge * particles[j].charge) * invSquare;

    particles[i].a -= a / particles[i].mass;
    particles[j].a += a / particles[j].mass;
}
inline void applyElectricForce(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j, Constants phyConsts) {
    double E = 0.5 * planes[j].sigma / phyConsts.E0;
    vec3 a = particles[i].charge * E * planes[j].normal;

    particles[i].a += a / particles[i].mass;
}
inline void applyUniformElectricForce(std::vector<Particle>& particles, int i, Constants phyConsts) {
    particles[i].a += phyConsts.E;
}


#endif