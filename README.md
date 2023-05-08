# The Plazza - Pizzeria Simulation

Pizzeria Simulation is a project that aims to simulate a pizzeria with a reception, multiple kitchens, cooks, and pizza cooking processes. The simulation includes load balancing, process/thread synchronization, and communication between different components of the system.
  

Table of contents
=================

<!--ts-->
   * [Tools](#Tools)
   * [Getting_Started](#Getting_Started)
      * [Instalation](#Instalation)
      * [Usage](#Usage)
   * [Built_With](#Built_With)
   * [Contributions](#contributions)
<!--te-->

Tools
=====

* Processes (man fork, man exit, man wait, man ...)
* Inter-process communication (IPC)
* STL threads
* POSIX threads (man pthread_*)

Getting_Started
===============

Follow these instructions to get a copy of the project up and running on your local machine for development and testing purposes.

Installation
------------

* Clone the repository:

```shell
git clone git@github.com:EpitechPromo2026/B-CCP-400-LYN-4-1-theplazza-jules.dutel.git
```

* Change to the project directory:
```shell
cd B-CCP-400-LYN-4-1-theplazza-jules.dutel
```

* Compile the project:
```shell 
make re
```

Usage
=====

```shell
./plazza <multiplier> <cooks_per_kitchen> <time>
```
- `<multiplier>`: multiplier for the cooking time of the pizzas.
- `<cooks_per_kitchen>`: The port number on which the server socket listens.
- `<time>`:  time in milliseconds, used by the kitchen stock to replace ingredients.

Built_With
==========

- C++ programming language
- POSIX threads
- STL threads
- Inter-process communication (IPC)

Credits
=======

* Kevin CARTTIGUEANE: 

<a href="https://www.linkedin.com/in/kevin-carttigueane-4798a9227/"><img src="https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white"></a>
<a href="https://github.com/Kcarttigueane"><img src="https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white"></a>


* Jules DUTEL: 

<a href="https://www.linkedin.com/in/jules-dutel/"><img src="https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white"></a>
<a href="https://github.com/jvlxz"><img src="https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white"></a>

* Thomas HEILES:

<a href="https://github.com/ThomasHls"><img src="https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white"></a>

