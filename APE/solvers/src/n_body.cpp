#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>

#include "bespokeConstructs.cpp"



struct Particle {
    double mass;
    vec3 s, v, a;

    Particle(double mass, const vec3& s, const vec3& v) : mass(mass), s(s), v(v) {
        a = 0;
    }

};


const double G = 6.67430e-11;

void computeForces(std::vector<Particle>& particles) {
    for (Particle& p : particles) {
        p.a = vec3(0, 0, 0);
    }

    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;

            double r_squared = dot(d, d);
            double r = std::sqrt(r_squared);

            if (r > 1e-100) {
                vec3 a = (G * particles[i].mass * particles[j].mass) / (r_squared * r) * d;

                particles[i].a += a / particles[i].mass;
                particles[j].a -= a / particles[j].mass;
            }
        }
    }
}

//leapfrog algo
void updateParticles(std::vector<Particle>& particles, double dt) {
    for (Particle& p : particles) {
        p.v += p.a * (dt / 2);
        p.s += p.v * dt;
    }

    computeForces(particles);

    for (Particle& p : particles) {
        p.v += p.a * (dt / 2); 
    }
}

void printParticle(const Particle& p) {
    std::cout << p.s.x << " " << p.s.y << " " << p.s.z << " " << p.v.x << " " << p.v.y << " " << p.v.z;
}

void yieldFrame(std::vector<Particle> particles) {
    for (const Particle& p : particles) {
        printParticle(p);
        std::cout << " ";
    }
    std::cout << "\n" << std::flush;
}


int main() {
    std::vector<Particle> particles;
    std::string input;
    double dt;
    int max_no_frames;
    double mass, px, py, pz, vx, vy, vz;

    while (true) {
        particles.clear();

        std::getline(std::cin, input);
        std::istringstream iss(input);

        iss >> dt >> max_no_frames;
        while (iss >> mass >> px >> py >> pz >> vx >> vy >> vz) {
            particles.emplace_back(mass, vec3(px, py, pz), vec3(vx, vy, vz));
        }

        for (int i = 0; i < max_no_frames; i++) {
            yieldFrame(particles);

            computeForces(particles);
            updateParticles(particles, dt);
        }
    }

    return 0;
}

