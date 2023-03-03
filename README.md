# Tiramisu Robot Software

The tiramisu robot software contains all the code running on the Wombat controller of FrenchBakery's Tiramisu (Create-based) robot.

## Features

 - Arm inverse kinematics
 - Gripper inverse kinematics
 - Basic driving functionality


## Getting started

This repo uses the frenchbakery/dev_template build system. To get started running this project, clone the repo, setup your workspace and compile/run the code according to the guide in the [dev_template's readme](https://github.com/frenchbakery/dev_template/blob/main/README.md).

## Clarification of source code structure

Not all code in this repository was written by FrenchBakery (Team HTL-Anichstraße #0620) for the Botball turnament specifically. This section is to clarify what is and is not part of our Botball code:

 - ```src/```: This is the main project folder and contains all the source files and headers specifically for controlling the Tiramisu (and possibly some code shared with the Croissant robot)
 - ```devinclude/```: ANy headers in this folder are NOT part of the project. This folder includes libkipr headers that are not included in compilation. They are purely required for VSCode intellisense support.
 - ```include/```: These are headers and sources for either third party or custom librarys that we developed ourselves and are used by the code. Currently, this project does not use any libraries jet.

## Members

There are 4 Members who have the permission to contribute to this repo. Any personal repository (linked in the form of a submodule) owned by any of the following accounts were also written by us:

 - [melektron](https://github.com/melektron)
 - [Nilusink](https://github.com/Nilusink)
 - [LiloZz](https://github.com/LiloZz)
 - [LukasKrah](https://github.com/LukasKrah)
