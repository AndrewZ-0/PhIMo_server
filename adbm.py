import sqlite3
import bcrypt
import uuid

class DatabaseManager:
    def __init__(self, dbName = "phimo.db"):
        self.__conn = sqlite3.connect(dbName, check_same_thread = False) #creates db if it does not exist
        self.__cursor = self.__conn.cursor()
        self.__create_tables()
    
    def __create_tables(self):
        #only executes create queries if tables have not been created already
        self.__cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS users (
                userId TEXT PRIMARY KEY, 
                username TEXT UNIQUE NOT NULL, 
                password TEXT NOT NULL, 
                email TEXT UNIQUE NOT NULL
            );
            """
        )
        self.__cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS projects (
                userId TEXT NOT NULL,
                projectName TEXT NOT NULL,
                creationTime TEXT NOT NULL,
                lastAccessed TEXT,
                PRIMARY KEY (userId, projectName),
                FOREIGN KEY (userId) REFERENCES users(userId)
            );
            """
        )
        self.__cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS simulations (
                userId TEXT NOT NULL,
                projectName TEXT NOT NULL,
                simulationName TEXT NOT NULL,
                creationTime TEXT NOT NULL,
                lastAccessed TEXT,
                PRIMARY KEY (userId, projectName, simulationName),
                FOREIGN KEY (userId) REFERENCES users(userId),
                FOREIGN KEY (projectName) REFERENCES projects(projectName)
            );
            """
        )
    

    #quick fix for fetchall being annoying: (since fetchall returns a list of tuples)
    def __fetchallTolist(self, data):
        return [row for row in data]
    
    #client facing
    def create_user(self, username, password, email):
        self.__cursor.execute(
            """
            SELECT userId FROM users 
                WHERE email = ?
            """, 
            (email, )
        )
        if self.__cursor.fetchone():
            return {"status": "ERR", "message": "Email already linked to another user"}

        try:
            password_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt())
            #very likely a random number: using as a unique representation of authentication certificates
            userId = str(uuid.uuid4())
            self.__cursor.execute(
                """
                INSERT INTO users (userId, username, password, email) 
                    VALUES (?, ?, ?, ?)
                """, 
                (userId, username, password_hash, email)
            )
            self.__conn.commit()

            return {"status": "OK", "certificate": userId}
        except sqlite3.IntegrityError:
            return {"status": "ERR", "message": "Username already exists"}

    #client facing
    def validate_user(self, username, password):
        self.__cursor.execute(
            """
            SELECT userId, password FROM users 
                WHERE username = ?
            """, 
            (username, )
        )
        result = self.__cursor.fetchone()

        if result and bcrypt.checkpw(password.encode("utf-8"), result[1]):
            return {"status": "OK", "certificate": result[0]}
        return {"status": "ERR", "message": "Invalid username or password"}
    
    #backend facing
    def validate_userId(self, userId):
        self.__cursor.execute(
            """
            SELECT userId FROM users 
                WHERE userId = ?
            """, 
            (userId, )
        )
        return self.__cursor.fetchone() is not None

    #client facing
    def list_projectNames(self, userId):
        self.__cursor.execute(
            """
            SELECT projectName FROM projects 
                WHERE userId = ?
                ORDER BY lastAccessed DESC
            """, 
            (userId, )
        )

        projectNames = self.__fetchallTolist(self.__cursor.fetchall())
        return {"status": "OK", "data": projectNames}

    #client facing
    def get_projectData(self, userId, projectName):
        self.__cursor.execute(
            """
            SELECT creationTime, lastAccessed FROM projects 
                WHERE userId = ? 
                    AND projectName = ?
            """, 
            (userId, projectName)
        )

        result = self.__cursor.fetchone()
        if not result:
            return {"status": "ERR", "message": "Project does not exist"}
        
        return {"status": "OK", "data": {
            "creationDate": result[0],
            "lastAccessed": result[1]
        }}

    #client facing
    def list_project_simulations(self, userId, projectName):
        self.__cursor.execute(
            """
            SELECT simulationName FROM simulations 
                WHERE userId = ?
                    AND projectName = ? 
                ORDER BY lastAccessed DESC
            """, 
            (userId, projectName)
        )

        simulationNames = self.__fetchallTolist(self.__cursor.fetchall())
        return {"status": "OK", "data": simulationNames}

    def get_simulationData(self, userId, projectName, simulationName):
        self.__cursor.execute(
            """
            SELECT creationTime, lastAccessed FROM simulations 
                WHERE userId = ?
                    AND projectName = ? 
                    AND simulationName = ?
            """, 
            (userId, projectName, simulationName)
        )

        result = self.__cursor.fetchone()
        if not result:
            return {"status": "ERR", "message": "Simulation does not exist"}
        
        return {"status": "OK", "data": {
            "creationDate": result[0],
            "lastAccessed": result[1]}
        }


    def projectName_exists(self, userId, projectName):
        self.__cursor.execute(
            """
            SELECT projectName FROM projects 
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (userId, projectName)
        )

        return self.__cursor.fetchone() is not None


    #client facing
    def create_project(self, userId, projectName, creationTime, lastAccessed):

        self.__cursor.execute(
            """
            INSERT INTO projects (userId, projectName, creationTime, lastAccessed)
                VALUES (?, ?, ?, ?)
            """, 
            (userId, projectName, creationTime, lastAccessed)
        )
        self.__conn.commit()

        return {"status": "OK"}

    #client facing
    def delete_project(self, userId, projectName):
        if not self.projectName_exists(userId, projectName):
            return {"status": "ERR", "message": "Project does not exist"}
        
        self.__cursor.execute(
            """
            DELETE FROM projects 
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (userId, projectName)
        )
        self.__cursor.execute(
            """
            DELETE FROM simulations 
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (userId, projectName)
        )
        self.__conn.commit()

        return {"status": "OK"}
    
    def simulationName_exists(self, userId, projectName, simulationName):
        self.__cursor.execute(
            """
            SELECT simulationName FROM simulations 
                WHERE userId = ?
                    AND projectName = ?
                    AND simulationName = ?
            """, 
            (userId, projectName, simulationName)
        )

        return self.__cursor.fetchone() is not None

    #client facing
    def create_simulation(self, userId, projectName, simulationName, creationTime, lastAccessed):
        if self.simulationName_exists(userId, projectName, simulationName):
            return {"status": "ERR", "message": "Simulation name already in use"}
        
        self.__cursor.execute(
            """
            INSERT INTO simulations (userId, projectName, simulationName, creationTime, lastAccessed)
                VALUES (?, ?, ?, ?, ?)
            """, 
            (userId, projectName, simulationName, creationTime, lastAccessed)
        )
        self.__conn.commit()

        return {"status": "OK"}
    
    #client facing
    def delete_simulation(self, userId, projectName, simulationName):
        if not self.simulationName_exists(userId, projectName, simulationName):
            return {"status": "ERR", "message": "Simulation does not exist"}
        
        self.__cursor.execute(
            """
            DELETE FROM simulations 
                WHERE userId = ? 
                    AND projectName = ?
                    AND simulationName = ?
            """, 
            (userId, projectName, simulationName)
        )
        self.__conn.commit()

        return {"status": "OK"}
    
    #client facing
    def rename_project(self, userId, oldProjectName, newProjectName):
        if not self.projectName_exists(userId, oldProjectName):
            return {"status": "ERR", "message": "Old project does not exist"}
        
        if self.projectName_exists(userId, newProjectName):
            return {"status": "ERR", "message": "New project name already in use"}

        self.__cursor.execute(
            """
            UPDATE projects 
                SET projectName = ?
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (newProjectName, userId, oldProjectName)
        )

        self.__cursor.execute(
            """
            UPDATE simulations 
                SET projectName = ?
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (newProjectName, userId, oldProjectName)
        )

        self.__conn.commit()

        return {"status": "OK"}
    
    def update_project_lastAccessed(self, userId, projectName, lastAccessed):
        self.__cursor.execute(
            """
            UPDATE projects 
                SET lastAccessed = ?
                WHERE userId = ?
                    AND projectName = ?
            """, 
            (lastAccessed, userId, projectName)
        )
        self.__conn.commit()
    
    def update_simulation_lastAccessed(self, userId, projectName, simulationName, lastAccessed):
        self.__cursor.execute(
            """
            UPDATE simulations
                SET lastAccessed = ?
                WHERE userId = ?
                    AND projectName = ?
                    AND simulationName = ?
            """, 
            (lastAccessed, userId, projectName, simulationName)
        )
        self.__conn.commit()
    
    def rename_simulation(self, userId, projectName, oldSimulationName, newSimulationName):
        if not self.simulationName_exists(userId, projectName, oldSimulationName):
            return {"status": "ERR", "message": "Old simulation does not exist"}

        if self.simulationName_exists(userId, projectName, newSimulationName):
            return {"status": "ERR", "message": "New simulation name already in use"}

        self.__cursor.execute(
            """
            UPDATE simulations 
                SET simulationName = ?
                WHERE userId = ?
                    AND projectName = ?
                    AND simulationName = ?
            """, 
            (newSimulationName, userId, projectName, oldSimulationName)
        )
        self.__conn.commit()

        return {"status": "OK"}
    
    def close(self):
        self.__conn.close()



if __name__ == '__main__':
    db_manager = DatabaseManager()
    
    print(db_manager.create_user("admin", "1234", "test@gmail.com"))