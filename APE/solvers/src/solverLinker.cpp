
#include "../headers/physics.h"
#include "../headers/impulseCollisionResolution.h"

class SolverLinker {
    public:
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, Constants)> activeCollisionFunctions;
        std::vector<void (*)(std::vector<Particle>&, int, int, const vec3, vec3, Constants)> activePOPForces;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, int, int, vec3, Constants)> activePlaneForces;
        Constants phyConsts;

        void linkCollision(const double e) {
            activeCollisionFunctions.push_back(handleCollisions);
            this->phyConsts.setE(e);
        }

        void linkGravity(const double G) {
            activePOPForces.push_back(applyGravity);
            this->phyConsts.setG(G);
        }

        void linkEForce(const double E0) {
            activePOPForces.push_back(applyElectricForce);
            activePlaneForces.push_back(applyElectricForce);
            this->phyConsts.setE0(E0);
        }

        void linkMForce(const double M0) {
            activePOPForces.push_back(applyMagneticForce);
            this->phyConsts.setM0(M0);
        }

        void clear() {
            activeCollisionFunctions.clear();
            activePOPForces.clear();
            activePlaneForces.clear();
        }

        void applyPOPForces(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d) {
            for (auto& forceFunction : this->activePOPForces) {
                forceFunction(particles, i, j, invSquare, d, this->phyConsts);
            }
        }

        void applyPlaneForces(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j, vec3 invSquare) {
            for (auto& forceFunction : this->activePlaneForces) {
                forceFunction(particles, planes, i, j, invSquare, this->phyConsts);
            }
        }

        void applyCollisions(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            for (auto& collisionFunction: this->activeCollisionFunctions) {
                collisionFunction(particles, planes, phyConsts); 
            }
        }
};
