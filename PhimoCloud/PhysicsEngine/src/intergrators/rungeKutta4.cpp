#include "../../headers/physics.h"
#include <functional>

void rungeKutta4_updateParticles(std::function<void (std::vector<Particle>&, std::vector<Plane>&)> computeForces, std::function<void (std::vector<Particle>&, std::vector<Plane>&)> applyCollisions, std::vector<Particle>& particles, std::vector<Plane>& planes, double dt) {
    int len = particles.size();
    std::vector<Particle> k1 = particles, k2 = particles, k3 = particles, k4 = particles;

    computeForces(k1, planes);
    for (int i = 0; i < len; i++) {
        k1[i].s = particles[i].v;
        k1[i].v = particles[i].a;
    }

    for (int i = 0; i < len; i++) {
        k2[i].s = particles[i].s + k1[i].s * (dt / 2);
        k2[i].v = particles[i].v + k1[i].v * (dt / 2);
    }
    computeForces(k2, planes);
    for (int i = 0; i < len; i++) {
        k2[i].v = k2[i].a; 
    }

    for (int i = 0; i < len; i++) {
        k3[i].s = particles[i].s + k2[i].s * (dt / 2);
        k3[i].v = particles[i].v + k2[i].v * (dt / 2);
    }
    computeForces(k3, planes);
    for (int i = 0; i < len; i++) {
        k3[i].v = k3[i].a;
    }

    for (int i = 0; i < len; i++) {
        k4[i].s = particles[i].s + k3[i].s * dt;
        k4[i].v = particles[i].v + k3[i].v * dt;
    }
    computeForces(k4, planes);
    for (int i = 0; i < len; i++) {
        k4[i].v = k4[i].a;
    }

    for (int i = 0; i < len; i++) {
        particles[i].s += (dt / 6.0) * (k1[i].s + 2 * k2[i].s + 2 * k3[i].s + k4[i].s);
        particles[i].v += (dt / 6.0) * (k1[i].v + 2 * k2[i].v + 2 * k3[i].v + k4[i].v);
    }

    applyCollisions(particles, planes);
}
