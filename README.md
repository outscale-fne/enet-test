# enet-test

## A simple multiplayer game POC written in pure C.

## What it does for now:

 * Players can join and leave the server.
 * Each movement are calculated both client side and server side.
 * Each player can move and see others moving.
 * Working fine accross the worldwide web.
 * Displaying a radar.

 ## What is missing:

 * Collision detection.
 * Shooting system.
 * Possibility to load custom spaceships via a json file i.e. (with validation from the server to avoid having too big vaissels).
 * Displaying a map given by the server (probably based on time).
 * Displaying in full screen and scaling game objects.


## Installation:

* `sudo apt install libsdl2-dev libenet-dev libsdl2-ttf-dev`
* `git clone https://github.com/Fran6nd/enet-test.git && cd enet-test && git submodule update --init`
* `make all`

## Usage:

* `./server`: run the server.

* `./client name ip`: run the client. Default name is "NewPlayer" and default ip is "localhost".

## Licence:

* See `LICENCE` file.
* About the font `WhiteRabbit-47pD.ttf`, I got it as a completely free font. Let me know if there's a mistake.