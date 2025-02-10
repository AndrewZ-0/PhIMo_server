#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

#include "bespokeConstructs.cpp"


struct Particle {
    double mass, radius;
    vec3 s, v, a;

    Particle(double mass, const vec3& s, const vec3& v, double radius) : mass(mass), s(s), v(v), radius(radius) {
        a = 0;
    }
};



const double G = 6.67430e-11;
const double e = 1.0;

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


void handleCollisions(std::vector<Particle>& particles) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;
            double distance = length(d);

            double overlap = (particles[i].radius + particles[j].radius) - distance;
            if (overlap > 0) {
                vec3 du = particles[j].v - particles[i].v;
                vec3 normal = d / distance; 

                if (dot(du, normal) < 0) {
                    double k = (1 + e) / (particles[i].mass + particles[j].mass);
                    vec3 dv = k * du;

                    particles[i].v += dv * particles[j].mass;
                    particles[j].v -= dv * particles[i].mass;
                }
            }
        }
    }
}


void updateParticles(std::vector<Particle>& particles, double dt) {
    for (Particle& p : particles) {
        p.v += p.a * (dt / 2);
        p.s += p.v * dt;
    }

    computeForces(particles);

    for (Particle& p : particles) {
        p.v += p.a * (dt / 2); 
    }

    handleCollisions(particles); 
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
    double mass, px, py, pz, vx, vy, vz, radius;

    while (true) {
        particles.clear();

        std::getline(std::cin, input);
        std::istringstream iss(input);
        
        iss >> dt >> max_no_frames;
        while (iss >> mass >> px >> py >> pz >> vx >> vy >> vz) {
            radius = 1.0;
            particles.emplace_back(mass, vec3(px, py, pz), vec3(vx, vy, vz), radius);
        }

        for (int i = 0; i < max_no_frames; i++) {
            yieldFrame(particles);
            updateParticles(particles, dt); 
        }
    }

    return 0;
}
