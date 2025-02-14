import subprocess

class SolverHandler:
    def __init__(self):
        subprocess.run(["g++", "-std=c++11", "-o", f"APE/solvers/exe/solverRunner", f"APE/solvers/src/solverRunner.cpp"], check = True)
    
        self.process = subprocess.Popen(
            [f"APE/solvers/exe/solverRunner"],
            stdin = subprocess.PIPE,
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE,
            text = True
        )

    def parseConfigs(self, simConfigs):
        deltaT = simConfigs["deltaT"]
        noOfFrames = simConfigs["noOfFrames"]
        stepsPerFrame = simConfigs["stepsPerFrame"]
        models = simConfigs["models"]

        input_data = f"{deltaT} {noOfFrames} {stepsPerFrame}"

        #collision, gravity, electric, magnetic
        collision = models["collisions"]
        gravity = models["gravity"]
        eForce = models["eForce"]
        mForce = models["mForce"]

        collisionToggle = int(collision["compute"])
        gravityToggle = int(gravity["compute"])
        eForceToggle = int(eForce["compute"])
        mForceToggle = int(mForce["compute"])

        input_data += f" {collisionToggle}"
        if (collisionToggle):
            input_data += f" {collision['e']}"
        input_data += f" {gravityToggle}"
        if (gravityToggle):
            input_data += f" {gravity['G']}"
        input_data += f" {eForceToggle}"
        if (eForceToggle):
            input_data += f" {eForce['E0']}"
        input_data += f" {mForceToggle}"
        if (mForceToggle):
            input_data += f" {mForce['M0']}"
            
        for obj in simConfigs["objects"].values():
            dtype = obj["dtype"]
            if dtype == 0:
                mass = obj["mass"]
                charge = obj["charge"]
                radius = obj["radius"]
                s = obj["position"]
                v = obj["velocity"]
                input_data += f" {dtype} {mass} {charge} {radius} {s[0]} {s[1]} {s[2]} {v[0]} {v[1]} {v[2]}"
            elif dtype == 1:
                #charge width height sx xy xz pitch yaw roll
                charge = obj["charge"]
                s = obj["position"]
                o = obj["orientation"] #orientations
                d = obj["dimentions"] #dimentions
                
                input_data += f" {dtype} {charge} {d[0]} {d[1]} {s[0]} {s[1]} {s[2]} {o[0]} {o[1]} {o[2]}"
        input_data += "\n"

        return input_data, noOfFrames

    
    def compute(self, simConfigs):
        input_data, noOfFrames = self.parseConfigs(simConfigs)

        self.process.stdin.write(input_data)
        self.process.stdin.flush()

        try:
            for i in range(1, noOfFrames):
                frame = self.process.stdout.readline()

                yield frame, i / noOfFrames
            frame = self.process.stdout.readline()[ :-1] #to removing trailing /n
            yield frame, 1
        except GeneratorExit:
            self.terminate()
            return
            
    def terminate(self):
        self.process.terminate()


