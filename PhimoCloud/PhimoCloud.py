from threading import Thread, Lock
import uuid
import time

from PhimoCloud.engineHandler import EngineHandler, ProcessHandler


class Worker:
    def __init__(self, engineHandler, workerId, userId, projectName, simulationName):
        self.workerId = workerId
        self.userId = userId
        self.projectName = projectName
        self.simulationName = simulationName
        self.computing = False #kinda redundant since object only lives for a single computation
        self.progress = 0
        self.end_time = None

        self.processHandler = ProcessHandler(engineHandler)
        self.processHandler.startProcess()

    def compute(self, workerCompletionCallback, outputCallback, simulationConfigs):
        self.computing = True

        particles = []
        for objName in simulationConfigs["objects"].keys():
            if simulationConfigs["objects"][objName]["dtype"] == 0:
                particles.append(objName)

        header_data = " ".join(particles) + "\n"
        outputCallback(self.workerId, header_data)

        self.taskGenerator = self.processHandler.compute(simulationConfigs)
        for frame, progress in self.taskGenerator:
            outputCallback(self.workerId, frame) #with \n char included
            self.progress = progress

        self.computing = False
        workerCompletionCallback(self.workerId)

    def terminate(self):
        self.processHandler.terminate()
        self.computing = False
        self.end_time = time.time()


class PhimoCloud:
    def __init__(self, file_manager):
        self.file_manager = file_manager
        self.workers = {}
        self.finishedWorkers = {}

        self.lock = Lock()

        self.max_no_of_workers = 8 #to prevent dos attacks

        #to prevent memory leak, workers are considered orphans if they are not claimed within 24 hours of task completion
        self.worker_lifetime = 86400 #24 hour
        self.cleanup_interval = 600 #every 10 mins

        self.engineHandler = EngineHandler()
        self.engineHandler.compile()

        self.cleanup_thread = Thread(target = self.cleanupOrphanedWorkers)
        self.cleanup_thread.daemon = True
        self.cleanup_thread.start()
    
    def compute_simulation(self, userId, projectName, simulationName):
        if len(self.workers) > self.max_no_of_workers:
            return {"status": "ERR", "message": "Physics engine worker stack is currently at full capacity. Try again later"}

        workerId = str(uuid.uuid4())
        worker = Worker(self.engineHandler, workerId, userId, projectName, simulationName)
        with self.lock:
            self.workers[workerId] = worker

        simulationConfigs, settings = self.file_manager.get_projectFiles(userId, projectName)

        workerThread = Thread(
            target = worker.compute, 
            args = (self.workerCompletionCallback, self.workerOutputCallback, simulationConfigs)
        )
        workerThread.start()

        return {"status": "OK", "workerId": workerId}

    def workerOutputCallback(self, workerId, data):
        worker = self.workers[workerId]
        self.file_manager.append_frame_to_simFile(worker.userId, worker.projectName, worker.simulationName, data)

    def workerCompletionCallback(self, workerId):
        worker = self.workers[workerId]
        #self.file_manager.load_simulationData(worker.userId, worker.projectName, worker.simulationName, worker.data)

        with self.lock:
            self.finishedWorkers[workerId] = worker
            self.workers.pop(workerId)

    def getComputationProgress(self, workerId):
        if workerId in self.workers.keys():
            return {"status": "OK", "progress": self.workers[workerId].progress}
        elif workerId in self.finishedWorkers.keys():
            return {"status": "OK", "progress": "COMPUTATION COMPLETE"}
        else:
            return {"status": "ERR", "message": "Invalid worker ID"}
    
    def cancel_computing_simulation(self, workerId):
        if workerId in self.workers:
            self.workers[workerId].terminate()
            #with self.lock:
                #self.workers.pop(workerId)

    def terminateAssociatedWorkers(self, userId, projectName, simulationName):
        with self.lock:
            workerNames = self.workers.values()
            for worker in workerNames:
                if worker.userId == userId and worker.projectName == projectName and worker.simulationName == simulationName:
                    if worker.computing:
                        worker.terminate()
                    
                        self.workers.pop(worker.workerId)
    
    def checkIfComputing(self, userId, projectName, simulationName):
        for worker in self.workers.values():
            if worker.userId == userId and worker.projectName == projectName and worker.simulationName == simulationName and worker.computing:
                return True
        
        return False

    def cleanupOrphanedWorkers(self):
        while True:
            current_time = time.time()
            for workerId, worker in list(self.workers.items()):
                if worker.end_time and (current_time - worker.end_time > self.worker_lifetime):
                    with self.lock:
                        self.workers.pop(workerId)
            time.sleep(self.cleanup_interval) 