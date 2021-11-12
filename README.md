# Proland_dev

My development clone of Inria's Proland: [Proland home page](http://proland.imag.fr/). Delbert forked from [LarsFlaeten/Proland_dev](https://github.com/LarsFlaeten/Proland_dev).

The goal is to update and modify the Proland code to be in accordance with OpenGL 3.3, and to work on a standard laptop with intel HD Graphics (Proland was earlier tested only on NVIDIA hardware).

## Introduction

Proland is a C++/OpenGL library for the real-time rendering of multi-resolution terrains (up to whole planets), the real-time management and edition of vector data (representing for instance roads or rivers), the rendering of atmosphere and clouds, the rendering and animation of the oceans, and the rendering of forests. All data are loaded or generated on the fly according to the viewpoint, and can be combined procedurally. For instance the shape and texture of a terrain can be modified using vector data representing roads and rivers.

Proland uses the Ork rendering library. You can find my version of ork here: [My Ork on github](https://github.com/cnDelbert/ork)

## Background

This clone of the Proland librabry is mainly meant to be able to build and run Proland libraries and examples on my Linux computer. The code hierarchy is largely maintained, but the build system is changed to CMake.

## Overview of the library

This directory contains the Proland core library, several "plugins" for this library, and demos using this library and plugins. Each directory 
has its own licence, source tree, examples, documentation and project files. The directories are mostly independent, except for some dependencies between plugins (and between all the plugins and the core library).

Proland is made of a core library and of 7 plugins:

* Core library
* Terrain plugin
* Atmo plugin
* Ocean plugin
* Forrest plugin
* Graph plugin
* River plugin
* Edit plugin

The original Code::Blocks project of Inria has been removed by LF.

## Screenshots

Some new screen shots using srtm/blue marble data sets

![alt text](web/blue_marble.png "Earth with clouds")

![alt text](web/italy.png "Italy")

![alt text](web/Pico_do_fogo.png "Cape Verde islands, Pico do Fogo")

Below is a screenshot of the use of precomputed atmospheric scattering.

![alt text](web/Atmo_test.png "Test of precomputed atmospheric scattering")

Below is a screenshot of the use of precomputed atmospheric scattering together with the ocean module, using the Ocean FFT algorithm.

![alt text](web/Ocean_and_atmo2.png "Test of precomputed atmospheric scattering and ocean")

![alt text](web/Procedural_landscape_test.png "Procedural landscape test")

![alt text](web/first_procedural_planet.png "First procedural planet")

![alt text](web/Blue_marble_test.png "First earth test (with low res texture)")

## Current status

* All modules are includes and now runs on a normal Linux computer with OpenGL 3.3
* Some issues still exists with the use of CPUBuffers from Ork
* Som bugs in the forest/trees rendering (These shaders where translated from Shader model 4 to 3.3 to work on my laptop) 

