#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

const double pi = 3.14159265358979;

struct Constants {
    double G; //gravitational constant
    double ke; //eForce factor
    double km; //mForce factor
    double e; //coefficient of restitution
    double rho; //air density

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

    inline void setRho(const double rho) {
        this->rho = rho;
    }
};

#endif
