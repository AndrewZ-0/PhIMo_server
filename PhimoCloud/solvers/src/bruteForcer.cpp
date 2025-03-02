#include "../headers/physics.h"
#include <functional>

void bruteForceComputePOPForces(std::function<void (std::vector<Particle>&, int, int, vec3, vec3)> applyPOPForces, std::vector<Particle>& particles) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;

            double r_squared = dot(d, d);
            double r = std::sqrt(r_squared);

            if (r < 1e-100) {
                continue;
            }

            vec3 invSquare = d / (r_squared * r);
            applyPOPForces(particles, i, j, invSquare, d);
        }
    }
}

void bruteForceComputePlaneForces(std::function<void (std::vector<Particle>&, std::vector<Plane>&, int, int)> applyPlaneForces, std::vector<Particle>& particles, std::vector<Plane>& planes) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < planes.size(); j++) {
            vec3 localPos = particles[i].s - planes[j].s;
            double r = dot(localPos, planes[j].normal);

            if (r < 0) {
                continue;
            }

            double xProj = dot(localPos, planes[j].xUnit); 
            double yProj = dot(localPos, planes[j].yUnit);
            if (abs(xProj) > planes[j].length / 2 || abs(yProj) > planes[j].width / 2) {
                continue;
            }

            applyPlaneForces(particles, planes, i, j);
        }
    }
}