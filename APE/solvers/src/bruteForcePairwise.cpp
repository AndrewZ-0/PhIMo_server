#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "physics.cpp"
#include "impulseCollisionResolution.cpp"


std::vector<Particle> particles;
std::vector<Plane> planes;
std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&)> activeCollisionFunctions;
std::vector<void (*)(std::vector<Particle>&, int, int, const vec3, vec3)> activePOPForces;
std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, int, int, vec3)> activePlaneForces;

void computeForces() {
    for (Particle& p : particles) {
        p.a = 0;
    }

    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;

            double r_squared = dot(d, d);
            double r = std::sqrt(r_squared);

            if (r > 1e-100) {
                vec3 invSquare = d / (r_squared * r);

                for (auto& forceFunction : activePOPForces) {
                    forceFunction(particles, i, j, invSquare, d);
                }
            }
        }
    }

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

            vec3 invSquare = planes[j].normal / (r * r);

            for (auto& forceFunction : activePlaneForces) {
                forceFunction(particles, planes, i, j, invSquare);
            }
        }
    }
}

//leapfrog
void updateParticles(double dt) {
    for (Particle& p : particles) {
        p.v += p.a * (dt / 2);
        p.s += p.v * dt;
    }

    computeForces();

    for (Particle& p : particles) {
        p.v += p.a * (dt / 2); 
    }

    for (auto& collisionFunction: activeCollisionFunctions) {
        collisionFunction(particles, planes); 
    }
}

void printParticle(const Particle& p) {
    std::cout << p.s.x << " " << p.s.y << " " << p.s.z << " " << p.v.x << " " << p.v.y << " " << p.v.z;
}

void yieldFrame() {
    for (const Particle& p : particles) {
        printParticle(p);
        std::cout << " ";
    }
    std::cout << "\n" << std::flush;
}

int main() {
    std::string input;
    double dt;
    int max_no_frames, stepsPerFrame;
    double mass, charge, sx, sy, sz, vx, vy, vz, radius, length, width, pitch, yaw, roll;
    int dtype;
    bool toggleCollision, toggleGravity, toggleEForce, toggleMForce;

    while (true) {
        particles.clear();
        planes.clear();
        activeCollisionFunctions.clear();
        activePOPForces.clear();
        activePlaneForces.clear();

        std::getline(std::cin, input);
        std::istringstream iss(input);
        
        iss >> dt >> max_no_frames >> stepsPerFrame >> toggleCollision >> toggleGravity >> toggleEForce >> toggleMForce;
        while (iss >> dtype) {
            if (dtype == 0) {
                iss >> mass >> charge >> radius >> sx >> sy >> sz >> vx >> vy >> vz;
                particles.emplace_back(mass, vec3(sx, sy, sz), vec3(vx, vy, vz), radius, charge);
            }
            else if (dtype == 1) {
                iss >> charge >> length >> width >> sx >> sy >> sz >> pitch >> yaw >> roll;
                planes.emplace_back(length, width, charge, vec3(sx, sy, sz), vec3(pitch, yaw, roll));
            }
        }


        if (toggleCollision) {
            activeCollisionFunctions.push_back(handleCollisions);
        }
        if (toggleGravity) {
            activePOPForces.push_back(applyGravity);
        }
        if (toggleEForce) {
            activePOPForces.push_back(applyElectricForce);
            activePlaneForces.push_back(applyElectricForce);
        }
        if (toggleMForce) {
            activePOPForces.push_back(applyMagneticForce);
        }

        dt /= stepsPerFrame;

        for (int i = 0; i < max_no_frames; i++) {
            yieldFrame();

            for (int j = 0; j < stepsPerFrame; j++) {
                updateParticles(dt); 
            }
        }
    }

    return 0;
}
