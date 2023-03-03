# Tiramisu Robot Software

The tiramisu robot software contains all the code running on the Wombat controller of FrenchBakery's Tiramisu (Create-based) robot.

## Features

 - Arm inverse kinematics
 - Gripper inverse kinematics
 - Basic driving functionality


## Getting started

This repo uses the frenchbakery/dev_template build system. To get started running this project, clone the repo and setup your workspace according to the guide in the [dev_template's readme](https://github.com/frenchbakery/dev_template/blob/main/README.md).

## Clarification of code structure

Not all code in this repository was written by FrenchBakery for the Botball turnament. This is to clarify what is and is not part of the Botball code.

 - ```src/```: This is the main project folder and contains all the source files and headers specifically for controlling the Tiramisu (and possibly some code shared with the Croissant robot)
 - ```devinclude/```: ANy headers in this folder are NOT part of the project. This folder includes libkipr headers that are not included in compilation. They are purely required for VSCode intellisense support.
 - ```include/
