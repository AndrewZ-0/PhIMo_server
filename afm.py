import os
import json
import shutil

class FileManager:
    def __init__(self):
        self.base_dir = "projects"
        if not os.path.exists(self.base_dir):
            os.makedirs(self.base_dir)

    def create_user_dir(self, userId):
        user_dir = os.path.join(self.base_dir, userId)
        os.makedirs(user_dir)

    def create_projectDir(self, userId, projectName):
        projectDir = os.path.join(self.base_dir, userId, projectName)
        os.makedirs(projectDir)

        with open(os.path.join(projectDir, "settings.json"), "w") as f:
            json.dump({}, f)

        with open(os.path.join(projectDir, "simConfig.json"), "w") as f:
            json.dump({}, f)

        os.makedirs(os.path.join(projectDir, "simulations"))

    def rename_projectDir(self, userId, old_projectName, new_projectName):
        old_projectDir = os.path.join(self.base_dir, userId, old_projectName)
        new_projectDir = os.path.join(self.base_dir, userId, new_projectName)
        os.rename(old_projectDir, new_projectDir)

    def delete_projectDir(self, userId, projectName):
        projectDir = os.path.join(self.base_dir, userId, projectName)
        if os.path.exists(projectDir):
            shutil.rmtree(projectDir) #nuke all the files in the project dir

    def delete_user_dir(self, userId):
        user_dir = os.path.join(self.base_dir, userId)
        if os.path.exists(user_dir):
            shutil.rmtree(user_dir) #nuke all the files in the user dir

    def get_projectFiles(self, userId, projectName):
        projectDir = os.path.join(self.base_dir, userId, projectName)

        with open(os.path.join(projectDir, "simConfig.json"), "r") as f:
            simConfig = json.load(f)
        with open(os.path.join(projectDir, "settings.json"), "r") as f:
            settings = json.load(f)
            
        return simConfig, settings
    
    def get_simulationConfig(self, userId, projectName, simulationName):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)

        with open(os.path.join(simulationDir, "simConfig.json"), "r") as f:
            simConfig = json.load(f)
        
        return simConfig
        
    def stream_simulationFramesFile(self, userId, projectName, simulationName):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)
        with open(os.path.join(simulationDir, "data.txt"), "r") as f:
            #I'm probably gonna hate myself for doing this considering how large these files can get :(
            #deepest apologies to whoever's RAM this function is frying (is what I would say, but now its streamed so it should be fine. I think...)
            
            for line in f:
                yield line

    def update_projectData(self, userId, projectName, simConfig):
        projectDir = os.path.join(self.base_dir, userId, projectName)

        with open(os.path.join(projectDir, "simConfig.json"), "w") as f:
            json.dump(simConfig, f, indent = 4)

    def save_screenshot(self, userId, projectName, screenshot):
        projectDir = os.path.join(self.base_dir, userId, projectName)
        screenshot_path = os.path.join(projectDir, "screenshot.png")

        with open(screenshot_path, "wb") as f:
            f.write(screenshot)

    def get_screenshot(self, userId, projectName):
        screenshot_path = os.path.join(self.base_dir, userId, projectName, "screenshot.png")
        if os.path.exists(screenshot_path):
            with open(screenshot_path, "rb") as f:
                return f.read()
        return None

    def create_simulation(self, userId, projectName, simulationName):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)
        os.makedirs(simulationDir, exist_ok = True)

        dataFile = os.path.join(simulationDir, "data.txt")
        with open(dataFile, "w") as f:
            f.write("")
        
        projectSimConfigPath = os.path.join(self.base_dir, userId, projectName, "simConfig.json")
        if os.path.exists(projectSimConfigPath):
            with open(projectSimConfigPath, "r") as f:
                simConfig = json.load(f)

            simulationSimConfigPath = os.path.join(simulationDir, "simConfig.json")
            with open(simulationSimConfigPath, "w") as f:
                json.dump(simConfig, f, indent = 4)

    def delete_simulation(self, userId, projectName, simulationName):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)
        
        if os.path.exists(simulationDir):
            shutil.rmtree(simulationDir)

    def rename_simulation(self, userId, projectName, old_simulationName, new_simulationName):
        old_simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", old_simulationName)
        new_simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", new_simulationName)
        
        if os.path.exists(old_simulationDir):
            os.rename(old_simulationDir, new_simulationDir)
    
    def load_simulationData(self, userId, projectName, simulationName, data):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)
        dataFile = os.path.join(simulationDir, "data.txt")

        if os.path.exists(dataFile):
            with open(dataFile, "w") as f:
                f.write(data)
    
    def append_frame_to_simFile(self, userId, projectName, simulationName, data):
        simulationDir = os.path.join(self.base_dir, userId, projectName, "simulations", simulationName)
        dataFile = os.path.join(simulationDir, "data.txt")

        if os.path.exists(dataFile):
            with open(dataFile, "a") as f:
                f.write(data)
