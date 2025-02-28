#pragma once
#ifndef DRAG_H
#define DRAG_H

#include <vector>
#include "bespokeConstructs.h"
#include <iostream>

inline void applyDrag(std::vector<Particle>& particles, int i, Constants phyConsts) {
    double speedSquared = dot(particles[i].v, particles[i].v);
    
    if (speedSquared == 0) {
        return;
    }

    double dragForce = 0.5 * phyConsts.rho * speedSquared * particles[i].Cd * particles[i].A;

    vec3 n = normalise(particles[i].v);
    vec3 a = (dragForce / particles[i].mass) * n;

    particles[i].a -= a;
}

#endif