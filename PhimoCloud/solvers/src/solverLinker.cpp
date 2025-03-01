#include "../headers/physics.h"

class SolverLinker {
    private: 
        //God I hate function pointers
        void removeAcitveGlobalFunction(std::vector<void (*)(std::vector<Particle>&, int, Constants)> functionList, void (*functionToRemove)(std::vector<Particle>&, int, Constants)) {
            functionList.erase(std::remove(functionList.begin(), functionList.end(), functionToRemove), functionList.end());
        }

    public:
        std::vector<void (*)(std::vector<Particle>&, Constants)> activePOPCollisionFunctions;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, Constants)> activePlaneCollisionFunctions;
        std::vector<void (*)(std::vector<Particle>&, int, int, const vec3, vec3, Constants)> activePOPForces;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, int, int, Constants)> activePlaneForces;
        std::vector<void (*)(std::vector<Particle>&, int, Constants)> activeGlobalForces;
        Constants phyConsts;

        void linkCollision(const double e) {
            activePlaneCollisionFunctions.push_back(handleCollisions);
            activePOPCollisionFunctions.push_back(handleCollisions);
            this->phyConsts.setE(e);
        }

        void linkGravity(const double G, vec3 g) {
            activePOPForces.push_back(applyGravity);
            this->phyConsts.setG(G);

            activeGlobalForces.push_back(applyUniformGravity);
            this->phyConsts.setg(g);
        }

        void linkEForce(const double E0, vec3 E) {
            activePOPForces.push_back(applyElectricForce);
            activePlaneForces.push_back(applyElectricForce);
            this->phyConsts.setE0(E0);

            activeGlobalForces.push_back(applyUniformElectricForce);
            this->phyConsts.setE(E);
        }

        void linkMForce(const double M0, vec3 B) {
            activePOPForces.push_back(applyMagneticForce);
            this->phyConsts.setM0(M0);

            activeGlobalForces.push_back(applyUniformMagneticForce);
            this->phyConsts.setB(B);
        }

        void linkDrag(const double rho) {
            activeGlobalForces.push_back(applyDrag);
            this->phyConsts.setRho(rho);
        }

        void clear() {
            activePOPCollisionFunctions.clear();
            activePlaneCollisionFunctions.clear();
            activePOPForces.clear();
            activePlaneForces.clear();
            activeGlobalForces.clear();
        }

        void applyPOPForces(std::vector<Particle>& particles, int i, int j, vec3 invSquare, vec3 d) {
            for (auto& forceFunction : this->activePOPForces) {
                forceFunction(particles, i, j, invSquare, d, this->phyConsts);
            }
        }

        void applyPlaneForces(std::vector<Particle>& particles, std::vector<Plane>& planes, int i, int j) {
            for (auto& forceFunction : this->activePlaneForces) {
                forceFunction(particles, planes, i, j, this->phyConsts);
            }
        }

        void applyCollisions(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            for (auto& collisionFunction: this->activePOPCollisionFunctions) {
                collisionFunction(particles, phyConsts); 
            }

            for (auto& collisionFunction: this->activePlaneCollisionFunctions) {
                collisionFunction(particles, planes, phyConsts); 
            }
        }

        void applyGlobalForces(std::vector<Particle>& particles, int i) {
            for (auto& forceFunction: this->activeGlobalForces) {
                forceFunction(particles, i, phyConsts); 
            }
        }

        void optimise(std::vector<Particle>& particles, std::vector<Plane>& planes) {
            if (! particles.size()) {
                activePOPForces.clear();
                activePOPCollisionFunctions.clear();
            }

            if (! planes.size()) {
                activePlaneForces.clear();
                activePlaneCollisionFunctions.clear();
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
