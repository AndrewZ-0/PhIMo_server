#include "../../headers/physics.h"
#include <functional>

//yet another comicly large function definition...
void leapfrog_updateParticles(std::function<void (std::vector<Particle>&, std::vector<Plane>&)> computeForces, std::function<void (std::vector<Particle>&, std::vector<Plane>&)> applyCollisions, std::vector<Particle>& particles, std::vector<Plane>& planes, double dt) {
    for (Particle& p : particles) {
        p.v += p.a * (dt / 2);
        p.s += p.v * dt;
    }

    computeForces(particles, planes);

    for (Particle& p : particles) {
        p.v += p.a * (dt / 2); 
    }

    applyCollisions(particles, planes);
}