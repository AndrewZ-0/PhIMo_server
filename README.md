Flask server and cloud engine of the PhIMo project (should be used together with PhIMo_client https://github.com/AndrewZ-0/PhIMo_client)

Contents:
* A Python Flask server to host AFM, ADBM and PhIMo Cloud
* PhIMo Cloud: A collection of ridged body solvers in C++ which could be configured client-side
* AFM: (Andrew's File Manager) A basic Python filing system for ADBM and other PhIMo Server functionalities
* ADBM: (Andrew's Database Manager) A Python SQLite 3 wrapper module which runs the login system 

PhIMo Cloud Capabilities:
* Brute force, Euler, Leapfrog and RK4 (unstable) ODE solvers
* Force solvers for: gravity, electric force, magnetic force, drag and impulse-based normal forces
* All solvers are toggleable and associated physical constants are editable

<img width="1440" height="650" alt="image" src="https://github.com/user-attachments/assets/6637cb0e-2307-49c8-bb8e-2cb7deb5ea44" />

<img width="1440" height="650" alt="image" src="https://github.com/user-attachments/assets/b7e4be24-cc8c-42b6-8ed1-e705b4524ec1" />
