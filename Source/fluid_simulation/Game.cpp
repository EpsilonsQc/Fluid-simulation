#include "Game.h"

#include <Code_Utilities_Light_v2.h>

using namespace std;
using namespace Core;

Game::Game() : _keepPlaying(true) {}

Game::~Game() {}

// Game Loop
// Reference: http://gameprogrammingpatterns.com/game-loop.html

void Game::loop()
{
    while (_keepPlaying) 
    {
        handleInput(); // Events
        update(); // Update
        render(); // Draw
    }
}
