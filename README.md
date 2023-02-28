<h1 align="center">Fluid simulation</h1>
<p align="center"><img width="900" src="https://user-images.githubusercontent.com/11299907/221718841-5fa9d242-86d8-4c1c-9524-0eb4682f571c.png"></p>
<p align="center"><img width="900" src="https://user-images.githubusercontent.com/11299907/221718852-ce843645-f9f1-4b34-afac-a57206360f63.png"></p>

---

## Introduction
A project made with the Raylib library, simulating smoothed-particle hydrodynamics. It was developed in the midst of my study in video game programming at College of Bois-de-Boulogne. This is one of the project made within the "Advanced programming" course (session 4 of 5).

## F.A.Q.

> What is the game engine used?
- Raylib library

> What is the development period?
- From October 17th 2022 to October 31th 2022 [14 days period]

## Controls
- Arrows (up/down/left/right): change direction of gravity
- Space bar: makes the fluid “explode” giving a random speed to the particles
- Numbers 1 to 9: restart the simulation with 1 to 5000 particles
- A, S, D keys: change the simulation display mode
- Right click: add a block of particles
- Left click: add a single particle
- C keys: change the color of the particles to a color chosen at random
- CTRL+Z: undo the last operation made
- CTRL+Shift+Z: reapply the operation that was just undone

## Credits
- [EpsilonsQc](https://github.com/EpsilonsQc) - various optimizations to improve performance, grid to visualize the number of particles in each cell, command pattern implementation (undo/redo)
- Smoothed-particle hydrodynamics simulation, based on Matthias Müller paper
  - https://matthias-research.github.io/pages/publications/sca03.pdf
- The code is derived from implementation under MIT license by Lucas V. Schuermann
  - https://github.com/cerrno/mueller-sph/tree/d24d025ce496db89de62ad4359bf89b175c712ed
  - Writeup: https://lucasschuermann.com/writing/implementing-sph-in-2d

## Legal
The source code in this repository is for non-commercial use only. If you use the source code you may not charge others for access to it or any derivative work thereof. All the assets (textures, sounds, etc.) were found on the internet (either purchased from the Asset Store or from other sources).
