
#include "../headers/physics.h"
#include "../headers/impulseCollisionResolution.h"

class SolverLinker {
    public:
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&)> activeCollisionFunctions;
        std::vector<void (*)(std::vector<Particle>&, int, int, const vec3, vec3)> activePOPForces;
        std::vector<void (*)(std::vector<Particle>&, std::vector<Plane>&, int, int, vec3)> activePlaneForces;

        SolverLinker() {}

        void link(bool toggleCollision, bool toggleGravity, bool toggleEForce, bool toggleMForce) {
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
        }

        void clear() {
            activeCollisionFunctions.clear();
            activePOPForces.clear();
            activePlaneForces.clear();
        }
    
};
