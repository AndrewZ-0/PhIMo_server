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

        #collision, gravity, electric, magnetic
        toggleCollision = int(models["collisions"])
        toggleGravity = int(models["gravity"])
        toggleEForce = int(models["eForce"])
        toggleMForce = int(models["mForce"])

        input_data = f"{deltaT} {noOfFrames} {stepsPerFrame} {toggleCollision} {toggleGravity} {toggleEForce} {toggleMForce}"

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
            for i in range(1, noOfFrames + 1):
                frame = self.process.stdout.readline()

                yield frame, i / noOfFrames
        except GeneratorExit:
            self.terminate()
            return
            
    def terminate(self):
        self.process.terminate()
    



