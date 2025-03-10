#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "solverLinker.cpp"

std::vector<Particle> particles;
std::vector<Plane> planes;

SolverLinker linker;

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
    int algorithm, intergrator;
    double mass, charge, Cd, sx, sy, sz, vx, vy, vz, radius, length, width, pitch, yaw, roll;
    int dtype;
    bool toggleCollision, toggleGravity, toggleEForce, toggleMForce, toggleDrag;
    double e, G, E0, M0, rho;
    vec3 g, E, B;

    while (true) {
        particles.clear();
        planes.clear();
        linker.clear();

        std::getline(std::cin, input);
        std::istringstream iss(input);
        
        iss >> dt >> max_no_frames >> stepsPerFrame;

        iss >> algorithm;
        if (algorithm == 0) {
            linker.configureBruteForcer();
        }
        
        iss >> intergrator;
        if (intergrator == 0) {
            linker.configureLeapfrog();
        }
        if (intergrator == 1) {
            linker.configureRungeKutta4();
        }
        if (intergrator == 2) {
            linker.configureEuler();
        }

        
        iss >> toggleCollision;
        if (toggleCollision) {
            iss >> e;
            linker.linkCollision(e);
        }
        iss >> toggleGravity;
        if (toggleGravity) {
            iss >> G >> g;
            linker.linkGravity(G, g);
        }
        iss >> toggleEForce;
        if (toggleEForce){
            iss >> E0 >> E;
            linker.linkEForce(E0, E);
        }
        iss >> toggleMForce;
        if (toggleMForce) {
            iss >> M0 >> B;
            linker.linkMForce(M0, B);
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

        linker.optimise(particles, planes);

        for (int i = 0; i < max_no_frames; i++) {
            yieldFrame();

            for (int j = 0; j < stepsPerFrame; j++) {
                linker.updateParticles(particles, planes, dt); 
            }
        }
    }

    return 0;
}
