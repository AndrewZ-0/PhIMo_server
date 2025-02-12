#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "bespokeConstructs.h"
#include <vector>


const double pi = 3.14159265359;

struct Constants {
    double G;
    double ke;
    double km;
    double e;

    inline void setG(const double G) {
        this->G = G;
    }

    inline void setE0(const double E0) {
        this->ke = 1 / (4 * pi * E0);
    }

    inline void setM0(const double M0) {
        this->km = M0 / (4 * pi);
    }

    inline void setE(const double e) {
        this->e = e;
    }


    //note to self: individually set consts and then apply indivially based on toggle
};

inline void applyGravity(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 a = (phyConsts.G * particles[i].mass * particles[j].mass) * invSquare;

    particles[i].a += a / particles[i].mass;
    particles[j].a -= a / particles[j].mass;
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


inline void applyMagneticForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d, Constants phyConsts) {
    vec3 alpha = phyConsts.km * particles[i].charge * particles[j].charge * invSquare;

    particles[i].a -= cross(particles[i].v, cross(particles[j].v, alpha)) / particles[i].mass;
    particles[j].a += cross(particles[j].v, cross(particles[i].v, alpha)) / particles[j].mass;
}

#endif