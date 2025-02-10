#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "bespokeConstructs.h"
#include <vector>

const double pi = 3.14159265359;

const double G = 6.6743015e-11;

inline void applyGravity(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d);


const double E0 = 8.854187817e-12;
const double ke = 1 / (4 * pi * E0);

inline void applyElectricForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d);
inline void applyElectricForce(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j, vec3 invSquare);

const double M0 = 1.25663706127e-6;
const double km = M0 / (4 * pi);

inline void applyMagneticForce(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d);

#endif