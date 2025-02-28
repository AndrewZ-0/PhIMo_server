#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "solverLinker.cpp"

std::vector<Particle> particles;
std::vector<Plane> planes;

SolverLinker linker;

void computeForces() {
    for (Particle& p : particles) {
        p.a = 0;
    }

    for (int i = 0; i < particles.size(); i++) {
        linker.applyGlobalForces(particles, i);
    }

    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;

            double r_squared = dot(d, d);
            double r = std::sqrt(r_squared);

            if (r > 1e-100) {
                vec3 invSquare = d / (r_squared * r);

                linker.applyPOPForces(particles, i, j, invSquare, d);
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

            linker.applyPlaneForces(particles, planes, i, j, invSquare);
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

    linker.applyCollisions(particles, planes);
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
    double mass, charge, Cd, sx, sy, sz, vx, vy, vz, radius, length, width, pitch, yaw, roll;
    int dtype;
    bool toggleCollision, toggleGravity, toggleEForce, toggleMForce, toggleDrag;
    double e, G, E0, M0, rho;

    while (true) {
        particles.clear();
        planes.clear();
        linker.clear();

        std::getline(std::cin, input);
        std::istringstream iss(input);
        
        iss >> dt >> max_no_frames >> stepsPerFrame;
        
        iss >> toggleCollision;
        if (toggleCollision) {
            iss >> e;
            linker.linkCollision(e);
        }
        iss >> toggleGravity;
        if (toggleGravity) {
            iss >> G;
            linker.linkGravity(G);
        }
        iss >> toggleEForce;
        if (toggleEForce){
            iss >> E0;
            linker.linkEForce(E0);
        }
        iss >> toggleMForce;
        if (toggleMForce) {
            iss >> M0;
            linker.linkMForce(M0);
        }
        iss >> toggleDrag;
        if (toggleDrag) {
            iss >> rho;
            linker.linkDrag(rho);
        }

        while (iss >> dtype) {
            if (dtype == 0) {
                iss >> mass >> charge >> Cd >> radius >> sx >> sy >> sz >> vx >> vy >> vz;
                particles.emplace_back(mass, vec3(sx, sy, sz), vec3(vx, vy, vz), radius, charge, Cd);
            }
            else if (dtype == 1) {
                iss >> charge >> length >> width >> sx >> sy >> sz >> pitch >> yaw >> roll;
                planes.emplace_back(length, width, charge, vec3(sx, sy, sz), vec3(pitch, yaw, roll));
            }
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
