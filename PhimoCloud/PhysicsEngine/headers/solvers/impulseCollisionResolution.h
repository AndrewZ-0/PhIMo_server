#pragma once
#ifndef IMPULSECOLLISIONRESOLUTION_H
#define IMPULSECOLLISIONRESOLUTION_H

#include <vector>
#include "../bespokeConstructs.h"

void handleCollisions(std::vector<Particle>& particles, Constants phyConsts) {
    for (int i = 0; i < particles.size(); i++) {
        for (int j = i + 1; j < particles.size(); j++) {
            vec3 d = particles[j].s - particles[i].s;
            double distance = length(d);

            //to prevent crashing if the centres of both particles are overlapping
            if (distance == 0) {
                break;
            }

            double overlap = (particles[i].radius + particles[j].radius) - distance;
            if (overlap > 0) {
                vec3 du = particles[j].v - particles[i].v;
                vec3 normal = d / distance; 
                double du_n = dot(du, normal);

                if (du_n < 0) {
                    double k = (1 + phyConsts.e) / (particles[i].mass + particles[j].mass);
                    vec3 dv = k * du_n * normal;

                    particles[i].v += dv * particles[j].mass;
                    particles[j].v -= dv * particles[i].mass;
                }
            }
        }
    }
}

void handleCollisions(std::vector<Particle>& particles, std::vector<Plane>& planes, Constants phyConsts) {
    for (Particle& p : particles) {
        for (Plane& plane : planes) {
            vec3 localPos = p.s - plane.s;
            double h = dot(localPos, plane.normal);

            double abs_h = abs(h);

            if (abs_h > p.radius) {
                continue;
            }

            double xProj = dot(localPos, plane.xUnit);
            double yProj = dot(localPos, plane.yUnit);
            if (abs(xProj) > plane.length / 2 || abs(yProj) > plane.width / 2) {
                continue;
            }

            double penetrationDepth = p.radius - abs_h;

            if (penetrationDepth > 0) {
                double velocityIntoPlane = dot(p.v, plane.normal);
                if ((h > 0 && velocityIntoPlane < 0) || (h < 0 && velocityIntoPlane > 0)) {
                    p.v -= (1 + phyConsts.e) * velocityIntoPlane * plane.normal;
                }
            }
        }
    }
}

#endif