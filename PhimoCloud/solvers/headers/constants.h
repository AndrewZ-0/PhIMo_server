#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "staticConstants.h"
#include "bespokeConstructs.h"

struct Constants {
    //universal
    double G; //gravitational constant
    double ke; //eForce factor
    double km; //mForce factor

    //global
    double e; //coefficient of restitution
    double rho; //air density
    vec3 g; //global gravitational field strength
    vec3 E; //global electric field strength
    vec3 B; //global magnetic field strength

    inline void setG(const double G) {
        this->G = G;
    }
    inline void setg(vec3 g) {
        this->g = g;
    }

    inline void setE0(const double E0) {
        this->ke = 1 / (4 * pi * E0);
    }
    inline void setE(vec3 E) {
        this->E = E;
    }

    inline void setM0(const double M0) {
        this->km = M0 / (4 * pi);
    }
    inline void setB(vec3 B) {
        this->B = B;
    }

    inline void setE(const double e) {
        this->e = e;
    }

    inline void setRho(const double rho) {
        this->rho = rho;
    }
};

#endif
