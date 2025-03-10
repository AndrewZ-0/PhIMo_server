from flask import Flask, render_template, request, jsonify, make_response
from flask_cors import CORS
import re
from datetime import datetime
import base64

#bespoke
from adbm import DatabaseManager
from afm import FileManager
from PhimoCloud.PhimoCloud import PhimoCloud

app = Flask(__name__)
CORS(app, supports_credentials = True)

db_manager = DatabaseManager()
file_manager = FileManager()
connected_clients = set()
phimoCloud = PhimoCloud(file_manager)




#legacy
"""
def handle_client_disconnect(client_ip):
    if client_ip in connected_clients:
        connected_clients.remove(client_ip)
        print(f"Client {client_ip} disconnected")
    
@app.route("/server_status", methods = ["GET"])
def server_status():
    return jsonify({"status": "RUNNING"})"
"""

@app.route("/connect", methods = ["GET"])
def handle_client_connect():
    client_ip = request.remote_addr
    connected_clients.add(client_ip)

    return jsonify({"response": "CONNECTED"})

#legacy
""" 
@app.route("/disconnect", methods = ["GET"])
def handle_client_disconnect_route():
    client_ip = request.remote_addr
    handle_client_disconnect(client_ip)

    return jsonify({"response": "DISCONNECTED"})"
"""

@app.route("/login", methods = ["POST"])
def login():
    data = request.json
    username = data["username"]
    password = data["password"]
    keepSignedIn = data["keepSignedIn"]

    validation_response = db_manager.validate_user(username, password)
    if validation_response["status"] == "OK":
        certificate = validation_response["certificate"]
        response = make_response(jsonify(validation_response))

        if keepSignedIn:
            response.set_cookie(
                "certificate", 
                certificate, 
                secure = True,
                httponly = True,
                samesite = "None"
            )
        return response
    else:
        return jsonify(validation_response)

@app.route("/logout", methods = ["POST"])
def logout():
    response = make_response(jsonify({"status": "OK"}))
    response.delete_cookie("certificate")

    return response

@app.route("/signup", methods = ["POST"])
def signup():
    data = request.json
    username = data["username"]
    password = data["password"]
    email = data["email"]

    response = db_manager.create_user(username, password, email)
    if response["status"] == "OK":
        user_id = response["certificate"]
        file_manager.create_user_dir(user_id)
    return jsonify(response)

@app.route("/validate_certificate", methods = ["POST"])
def validate_certificate():
    data = request.json
    certificate = data["certificate"]

    if db_manager.validate_userId(certificate):
        return jsonify({"status": "OK"})
    else:
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

@app.route("/validate_email", methods = ["POST"])
def validate_email():
    data = request.json
    email = data["email"]

    if re.match(r"^[\w\.-]+@[\w\.-]+\.\w+$", email):
        return jsonify({"status": "OK"})
    else:
        return jsonify({"status": "ERR", "message": "Invalid email format"})

@app.route("/validate_username", methods = ["POST"])
def validate_username():
    data = request.json
    username = data["username"]

    if re.match(r"^\w+$", username):
        return jsonify({"status": "OK"})
    else:
        return jsonify({"status": "ERR", "message": "Invalid username format"})

@app.route("/validate_password", methods = ["POST"])
def validate_password():
    data = request.json
    password = data["password"]

    if not re.match(r"^[\w!?@£#%$^&*<>,.€~]*$", password):
        return jsonify({"status": "ERR", "message": "Invalid character in password"})
    if len(password) < 4:
        return jsonify({"status": "ERR", "message": "Password too short"})
    
    return jsonify({"status": "OK"})
    

@app.route("/listProjects", methods = ["GET"])
def list_projects():
    userId = request.headers["certificate"]

    if db_manager.validate_userId(userId):
        return jsonify(db_manager.list_projectNames(userId))
    else:
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

@app.route("/get_projectData", methods = ["GET"])
def get_projectData():
    userId = request.headers["certificate"]
    projectName = request.headers["projectName"]

    if db_manager.validate_userId(userId):
        projectData = db_manager.get_projectData(userId, projectName)
        if projectData["status"] == "OK":
            simConfig, settings = file_manager.get_projectFiles(userId, projectName)
            projectData["data"]["simConfig"] = simConfig
            projectData["data"]["settings"] = settings
        return jsonify(projectData)
    else:
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

@app.route("/list_project_simulations", methods = ["GET"])
def list_project_simulations():
    userId = request.headers["certificate"]
    projectName = request.headers["projectName"]

    if db_manager.validate_userId(userId):
        return jsonify(db_manager.list_project_simulations(userId, projectName))
    else:
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
@app.route("/get_simulationData", methods = ["GET"])
def get_simulationData():
    headers = request.headers
    userId = headers["certificate"]
    projectName = headers["projectName"]
    simulationName = headers["simulationName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    simulationData = db_manager.get_simulationData(userId, projectName, simulationName)

    if simulationData["status"] == "OK":
        simConfig, settings = file_manager.get_simulationConfig(userId, projectName, simulationName)
        simulationData["data"]["simConfig"] = simConfig
        simulationData["data"]["settings"] = settings

    return jsonify(simulationData)


@app.route("/stream_simulationFramesFile", methods = ["GET"])
def stream_simulationFramesFile():
    headers = request.headers
    userId = headers["certificate"]
    projectName = headers["projectName"]
    simulationName = headers["simulationName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project not found"})
    
    if not db_manager.simulation_exists(userId, projectName, simulationName):
        return jsonify({"status": "ERR", "message": "Simulation not found"})
    
    response = make_response(file_manager.stream_simulationFramesFile(userId, projectName, simulationName))
    response.headers.set("Content-Type", "text/plain")

    return response

@app.route("/create_project", methods = ["POST"])
def create_project():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    creationTime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    lastAccessed = None

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not re.match(r"^\w+$", projectName):
        return jsonify({"status": "ERR", "message": "Invalid project name"})

    if db_manager.project_exists(userId, projectName): #moved here from adbm
        return jsonify({"status": "ERR", "message": "Project name already in use"})
    
    file_manager.create_projectDir(userId, projectName)

    response = db_manager.create_project(userId, projectName, creationTime, lastAccessed)

    return jsonify(response)

@app.route("/delete_project", methods = ["POST"])
def delete_project():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    simulations = db_manager.list_project_simulations(userId, projectName)["data"]
    for simulation in simulations:
        phimoCloud.terminateAssociatedWorkers(userId, projectName, simulation[0])
        db_manager.delete_simulation(userId, projectName, simulation[0])

    file_manager.delete_projectDir(userId, projectName)
    return jsonify(db_manager.delete_project(userId, projectName))

@app.route("/rename_project", methods = ["POST"])
def rename_project():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    oldProjectName = data["oldProjectName"]
    newProjectName = data["newProjectName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not re.match(r"^\w+$", newProjectName):
        return jsonify({"status": "ERR", "message": "Invalid project name"})

    response = db_manager.rename_project(userId, oldProjectName, newProjectName)
    if response["status"] == "OK":
        file_manager.rename_projectDir(userId, oldProjectName, newProjectName)
    return jsonify(response)

@app.route("/get_certificateCookie", methods = ["GET"])
def get_certificateCookie():
    certificate = request.cookies.get("certificate")

    return jsonify({"status": "OK", "certificate": certificate}) #using None to indicate no certificate found

@app.route("/update_accessProjectTime", methods = ["POST"])
def update_accessProjectTime():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    lastAccessed = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    db_manager.update_project_lastAccessed(userId, projectName, lastAccessed)

    return jsonify({"status": "OK"})

@app.route("/update_projectData", methods = ["POST"])
def update_projectData():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    simConfig = data["simConfig"]
    settingsData = data["settingsData"]
    screenshot_data_url = data["screenshot"]

    screenshot_base64 = screenshot_data_url.split(",")[1]
    screenshot = base64.b64decode(screenshot_base64)

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})

    file_manager.update_projectData(userId, projectName, simConfig, settingsData)
    file_manager.save_screenshot(userId, projectName, screenshot)

    return jsonify({"status": "OK"})

@app.route("/update_simulationSettings", methods = ["POST"])
def update_simulationSettings():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    simulationName = data["simulationName"]
    settingsData = data["settingsData"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not db_manager.simulation_exists(userId, projectName, simulationName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})

    file_manager.update_simulationSettings(userId, projectName, simulationName, settingsData)

    return jsonify({"status": "OK"})

@app.route("/get_projectScreenshot", methods = ["GET"])
def get_projectScreenshot():
    userId = request.headers["certificate"]
    projectName = request.headers["projectName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})
    
    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})

    screenshot = file_manager.get_screenshot(userId, projectName)
    if screenshot:
        response = make_response(screenshot)
        response.headers.set("Content-Type", "image/png")
        return response
    else:
        with open("assets/defaultProjectScreenshot.png", "rb") as f:
            fallback_image = f.read()
        response = make_response(fallback_image)
        response.headers.set("Content-Type", "image/png")
        return response


@app.route("/delete_simulation", methods = ["POST"])
def delete_simulation():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    simulationName = data["simulationName"]

    if phimoCloud.checkIfComputing(userId, projectName, simulationName):
        return jsonify({"status": "ERR", "message": "Simulation still computing cannot edit without WorkerID"})

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})
    
    phimoCloud.terminateAssociatedWorkers(userId, projectName, simulationName)

    file_manager.delete_simulation(userId, projectName, simulationName)
    return jsonify(db_manager.delete_simulation(userId, projectName, simulationName))

@app.route("/rename_simulation", methods = ["POST"])
def rename_simulation():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    oldSimulationName = data["oldSimulationName"]
    newSimulationName = data["newSimulationName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    if not re.match(r"^\w+$", newSimulationName):
        return jsonify({"status": "ERR", "message": "Invalid simulation name"})

    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})

    response = db_manager.rename_simulation(userId, projectName, oldSimulationName, newSimulationName)
    if response["status"] == "OK":
        file_manager.rename_simulation(userId, projectName, oldSimulationName, newSimulationName)
    return jsonify(response)

@app.route("/compute_simulation", methods = ["POST"])
def compute_simulation():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    simulationName = data["simulationName"]
    creationTime = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    lastAccessed = None

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    if not re.match(r"^\w+$", simulationName):
        return jsonify({"status": "ERR", "message": "Invalid simulation name"})
    
    if not db_manager.project_exists(userId, projectName):
        return jsonify({"status": "ERR", "message": "Project does not exist"})

    if db_manager.simulation_exists(userId, projectName, simulationName):
        return jsonify({"status": "ERR", "message": "Simulation name already in use"})

    file_manager.create_simulation(userId, projectName, simulationName)
    response = db_manager.create_simulation(userId, projectName, simulationName, creationTime, lastAccessed)

    if response["status"] != "OK":
        return jsonify(response)

    response = phimoCloud.compute_simulation(userId, projectName, simulationName)

    return jsonify(response)

@app.route("/stop_computing_simulation", methods = ["POST"])
def stop_computing_simulation():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    workerId = data["workerId"]
    projectName = data["projectName"]
    simulationName = data["simulationName"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    phimoCloud.cancel_computing_simulation(workerId)

    file_manager.delete_simulation(userId, projectName, simulationName)
    
    response = db_manager.delete_simulation(userId, projectName, simulationName)

    if response["status"] != "OK":
        return jsonify(response)

    return jsonify({"status": "OK"})

@app.route("/get_simComputing_progress", methods = ["GET"])
def get_simComputing_progress():
    userId = request.headers["certificate"]
    workerId = request.headers["workerId"]

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    response = phimoCloud.getComputationProgress(workerId)
    return jsonify(response)

@app.route("/update_accessSimulationTime", methods = ["POST"])
def update_accessSimulationTime():
    headers = request.headers
    data = request.json
    userId = headers["certificate"]
    projectName = data["projectName"]
    simulationName = data["simulationName"]
    lastAccessed = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    if not db_manager.validate_userId(userId):
        return jsonify({"status": "ERR", "message": "Invalid certificate"})

    db_manager.update_simulation_lastAccessed(userId, projectName, simulationName, lastAccessed)

    return jsonify({"status": "OK"})

@app.route("/list_solvers", methods = ["GET"])
def list_solvers():
    userId = request.headers["certificate"]

    if db_manager.validate_userId(userId):
        return jsonify(phimoCloud.list_solvers())
    else:
        return jsonify({"status": "ERR", "message": "Invalid certificate"})


serverPassword = "phimo123"

#server index page stuff
def restart_server():
    print("Restarting Server")
    

@app.route("/")
def home():
    return render_template("index.html")


if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 1234, debug = True, ssl_context = ("cert.pem", "key.pem"))