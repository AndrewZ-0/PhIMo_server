#include "../../headers/physics.h"
#include <functional>

void euler_updateParticles(std::function<void (std::vector<Particle>&, std::vector<Plane>&)> computeForces, std::function<void (std::vector<Particle>&, std::vector<Plane>&)> applyCollisions, std::vector<Particle>& particles, std::vector<Plane>& planes, double dt) {
    computeForces(particles, planes);
    
    for (Particle& p : particles) {
        p.s += p.v * dt;
        p.v += p.a * dt;
    }

    applyCollisions(particles, planes);
}