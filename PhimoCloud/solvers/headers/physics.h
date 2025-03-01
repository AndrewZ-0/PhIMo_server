#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

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

inline void applyElectricForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 a = (phyConsts.ke * particles[i].charge * particles[j].charge) * invSquare;

    particles[i].a -= a / particles[i].mass;
    particles[j].a += a / particles[j].mass;
}
inline void applyElectricForce(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j, vec3 invSquare, Constants phyConsts) {
    vec3 a = (phyConsts.ke * particles[i].charge * planes[j].charge) * invSquare;

    particles[i].a += a / particles[i].mass;
}
inline void applyUniformElectricForce(std::vector<Particle>& particles, int i, Constants phyConsts) {
    particles[i].a += phyConsts.E;
}

inline void applyMagneticForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 alpha = phyConsts.km * particles[i].charge * particles[j].charge * invSquare;

    particles[i].a -= cross(particles[i].v, cross(particles[j].v, alpha)) / particles[i].mass;
    particles[j].a += cross(particles[j].v, cross(particles[i].v, alpha)) / particles[j].mass;
}
inline void applyMagneticForce(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j, vec3 invSquare, Constants phyConsts) {
    //plane version in progress
    particles[i].a += 0;
}
inline void applyUniformMagneticForce(std::vector<Particle>& particles, int i, Constants phyConsts) {
    particles[i].a += cross(particles[i].v, phyConsts.B) / particles[i].mass;
}

#endif