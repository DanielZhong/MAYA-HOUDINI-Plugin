# MAYA C++ & Mel Plugin - L System

* Ruijun(Daniel) Zhong
    * [LinkedIn](https://www.linkedin.com/in/daniel-z-73158b152/)    
    * [Personal Website](https://www.danielzhongportfolio.com/)
* Tested on: 
  * Core(TM) i7-12700K 3.61 GHz 32.0 GB, NVIDIA GeForce RTX 3070 Ti (personal computer)   
    * Maya 2022

|<img src="LSystemResult2.jpg" width="100%">|<img src="LSystemResult.png" width="100%">|<img src="LSystemInstance (2).jpg" width="100%">|<img src="LSystemInstance (3).jpg" width="100%">|
|:-:|:-:|:-:|:-:|
|C++ Result|C++ Result|Python Result|Python Result|

## Introduction
This project encompasses the development of a Maya plugin designed to integrate L-Systems, a mathematical modeling language used to simulate the growth processes of plant development and other naturally occurring phenomena. The plugin is developed using a combination of C++ for the backend functionality and Maya Embedded Language (MEL) for the user interface and integration within Maya's ecosystem.

The core objective of this plugin is to provide Maya users with tools to generate and visualize complex organic structures through the procedural generation capabilities of L-Systems. By leveraging the powerful Maya Plugin API, this project adds custom commands and nodes to Maya, enabling the creation of intricate plant models and other fractal-like structures with high efficiency and flexibility.

## Plugin Demo
|<img src="LSystemMenu.jpg" width="100%">|<img src="LSystemCMD.jpg" width="100%">|<img src="LSystemNode.jpg" width="100%">|
|:-:|:-:|:-:|
|Menu|GUI|Node Interface|

## Loading Plugin in Maya
![](LSystemInstance.gif)
![](LSystemCMD.gif)
![](LSystemNode.gif)


## Implementation
Grammar and Production Rules: The input grammar consists of an initial axiom and a set of production rules. Each rule maps a character (symbol) to a string of symbols. For example, in the rule F -> FF+F-F+F, the symbol F is replaced by the string FF+F-F+F in each iteration. 

Turtle Interpretation: The algorithm uses a "turtle" to interpret the symbols in the generated string. Each symbol corresponds to an action:

* F: Move the turtle forward by a step size, drawing a line segment.
* f: Move the turtle forward without drawing.
* +: Rotate the turtle's heading to the left by a default angle.
* -: Rotate the turtle's heading to the right by the same angle.
* &, ^, \, /: Rotate the turtle around its left, forward, or up axes, enabling 3D movements.
* [: Save the current state of the turtle (position and orientation) onto a stack.
* ]: Pop the turtle's state from the stack, reverting to the last saved state.