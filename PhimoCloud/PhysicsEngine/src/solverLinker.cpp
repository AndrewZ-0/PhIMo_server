#include "../headers/physics.h"
#include "algorithms/bruteForcer.cpp"
#include "intergrators/leapfrog.cpp"
#include "intergrators/rungeKutta4.cpp"
#include "intergrators/euler.cpp"

void noOpComputePOPForces(std::function<void (std::vector<Particle>&, int, int, vec3, vec3)> applyPOPForces, std::vector<Particle>& particles) {}
void noOpComputePlaneForces(std::function<void (std::vector<Particle>&, std::vector<Plane>&, int, int)> applyPlaneForces, std::vector<Particle>& particles, std::vector<Plane>& planes) {}

class SolverLinker {
    private: 
        //God I hate function pointers
        void (* computePOPForces)(std::function<void(std::vector<Particle>&, int, int, vec3, vec3)>, std::vector<Particle>&);
        void (* computePlaneForces)(std::function<void(std::vector<Particle>&, std::vector<Plane>&, int, int)>, std::vector<Particle>&, std::vector<Plane>&);

        void (* particleUpdate_intergrator)(std::function<void(std::vector<Particle>&, std::vector<Plane>&)>, std::function<void(std::vector<Particle>&, std::vector<Plane>&)>, std::vector<Particle>&, std::vector<Plane>&, double);

        void removeAcitveGlobalFunction(std::vector<void (*)(std::vector<Particle>&, int, Constants)> functionList, void (*functionToRemove)(std::vector<Particle>&, int, Constants)) {
            functionList.erase(std::remove(functionList.begin(), functionList.end(), functionToRemove), functionList.end());
        }

    public:
        std::vector<void (*)(std::vector<Particle>&, Constants)> activePOPCollisionSolvers;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, Constants)> activePlaneCollisionSolvers;
        std::vector<void (*)(std::vector<Particle>&, int, int, const vec3, vec3, Constants)> activePOPSolvers;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, int, int, Constants)> activePlaneSolvers;
        std::vector<void (*)(std::vector<Particle>&, int, Constants)> activeGlobalForces;

        Constants phyConsts;

        //apply integrator
        void updateParticles(std::vector<Particle>& particles, std::vector<Plane>& planes, double dt) {
            this->particleUpdate_intergrator(
                [&](std::vector<Particle>& particles, std::vector<Plane>& planes) {
                    this->computeForces(particles, planes);
                }, 
                [&](std::vector<Particle>& particles, std::vector<Plane>& planes) {
                    this->applyCollisions(particles, planes);
                }, 
                particles, planes, dt
            );
        }

        void configureBruteForcer() {
            this->computePOPForces = bruteForceComputePOPForces;
            this->computePlaneForces = bruteForceComputePlaneForces;
        }

        void configureLeapfrog() {
            this->particleUpdate_intergrator = leapfrog_updateParticles;
        }

        void configureRungeKutta4() {
            this->particleUpdate_intergrator = rungeKutta4_updateParticles;
        }

        void configureEuler() {
            this->particleUpdate_intergrator = euler_updateParticles;
        }

        void linkCollision(const double e) {
            activePlaneCollisionSolvers.push_back(handleCollisions);
            activePOPCollisionSolvers.push_back(handleCollisions);
            this->phyConsts.sete(e);
        }

        void linkGravity(const double G, vec3 g) {
            activePOPSolvers.push_back(applyGravity);
            this->phyConsts.setG(G);

            activeGlobalForces.push_back(applyUniformGravity);
            this->phyConsts.setg(g);
        }

        void linkEForce(const double E0, vec3 E) {
            activePOPSolvers.push_back(applyElectricForce);
            activePlaneSolvers.push_back(applyElectricForce);
            this->phyConsts.setE0(E0);

            activeGlobalForces.push_back(applyUniformElectricForce);
            this->phyConsts.setE(E);
        }

        void linkMForce(const double M0, vec3 B) {
            activePOPSolvers.push_back(applyMagneticForce);
            this->phyConsts.setM0(M0);

            activeGlobalForces.push_back(applyUniformMagneticForce);
            this->phyConsts.setB(B);
        }

        void linkDrag(const double rho) {
            activeGlobalForces.push_back(applyDrag);
            this->phyConsts.setRho(rho);
        }

        void clear() {
            activePOPCollisionSolvers.clear();
            activePlaneCollisionSolvers.clear();
            activePOPSolvers.clear();
            activePlaneSolvers.clear();
            activeGlobalForces.clear();
        }

        void applyPOPForces(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d) {
            for (auto& forceFunction : this->activePOPSolvers) {
                forceFunction(particles, i, j, invSquare, d, this->phyConsts);
            }
        }

        void applyPlaneForces(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j) {
            for (auto& forceFunction : this->activePlaneSolvers) {
                forceFunction(particles, planes, i, j, this->phyConsts);
            }
        }

        void applyCollisions(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            for (auto& collisionFunction: this->activePOPCollisionSolvers) {
                collisionFunction(particles, phyConsts); 
            }

            for (auto& collisionFunction: this->activePlaneCollisionSolvers) {
                collisionFunction(particles, planes, phyConsts); 
            }
        }

        void applyGlobalForces(std::vector<Particle>& particles, int i) {
            for (auto& forceFunction: this->activeGlobalForces) {
                forceFunction(particles, i, phyConsts); 
            }
        }

        void computeForces(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            for (Particle& p : particles) {
                p.a = 0;
            }
        
            for (int i = 0; i < particles.size(); i++) {
                applyGlobalForces(particles, i);
            }
        
            this->computePOPForces(
                [&](std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d) {
                    applyPOPForces(particles, i, j, invSquare, d);
                }, 
                particles
            );
        
            this->computePlaneForces(
                [&](std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j) {
                    applyPlaneForces(particles, planes, i, j);
                }, 
                particles, planes
            );
        }

        void optimise(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            if (particles.empty()) {
                activePOPSolvers.clear();
                activePOPCollisionSolvers.clear();
                this->computePOPForces = noOpComputePOPForces;
            } 
        
            if (planes.empty()) {
                activePlaneSolvers.clear();
                activePlaneCollisionSolvers.clear();
                this->computePlaneForces = noOpComputePlaneForces;
            } 

            if (! this->phyConsts.g) {
                removeAcitveGlobalFunction(activeGlobalForces, applyUniformGravity);
            }

            if (! this->phyConsts.E) {
                removeAcitveGlobalFunction(activeGlobalForces, applyUniformElectricForce);
            }

            if (! this->phyConsts.B) {
                removeAcitveGlobalFunction(activeGlobalForces, applyUniformMagneticForce);
            }
        }
};
