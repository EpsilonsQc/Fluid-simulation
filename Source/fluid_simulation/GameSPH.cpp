#include "GameSPH.h"

#include <algorithm>
#include <raylib.h>
#include <Code_Utilities_Light_v2.h>

#include "Globals.h"
#include "ParticleManager.h"
#include "Commands.h"

using namespace std;

namespace SPH
{
    GameSPH::GameSPH()
        : _pause(false)
        , _nextCmdIndex(0)
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
        SetTargetFPS(FPS); // Set our game to run at 30 frames-per-second

        _particleManager.init(presets[1]);
    }

    GameSPH::~GameSPH()
    {
        CloseWindow();

        clearHistory(0);
        _nextCmdIndex = 0;
    }

    int GameSPH::getClickX()
    {
        return std::clamp(GetMouseX(), 0, SCREEN_WIDTH);
    }

    int GameSPH::getClickY()
    {
        return std::clamp(GetMouseY(), 0, SCREEN_WIDTH);
    }

    void GameSPH::handleInput()
    {
        // Mouse
        int x = getClickX(), y = getClickY();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            addCommand(new CmdAddGroup{_particleManager, x, y });
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            addCommand(new CmdAddOne{ _particleManager, x, y });

        // Key pressed
        int key = GetKeyPressed();
        switch (key)
        {
            // Control
        case KEY_SPACE:
            _particleManager.explode();
            break;
        case KEY_LEFT:
            _particleManager.setGravity(LEFT);
            break;
        case KEY_RIGHT:
            _particleManager.setGravity(RIGHT);
            break;
        case KEY_UP:
            _particleManager.setGravity(UP);
            break;
        case KEY_DOWN:
            _particleManager.setGravity(DOWN);
            break;

            // Display
        case KEY_A:
            _particleManager.setRenderMode((uchar)Render::Particles);
            break;
        case KEY_S:
            _particleManager.setRenderMode((uchar)Render::Particles | (uchar)Render::DrawGrid);
            break;
        case KEY_D:
            _particleManager.setRenderMode((uchar)Render::DrawGrid);
            break;

            // Game Handle
        case KEY_P:
            _pause = !_pause;
            break;
        case KEY_ESCAPE:
            _keepPlaying = false;
            break;
        case KEY_C:
            {
                const Color& c = _particleManager.getColor();
                auto* cmd = new CmdChangeColor{ _particleManager, c};
                cmd->setNew(BdB::randInt(255), BdB::randInt(255), BdB::randInt(255));
                addCommand(cmd);
            } break;

            // Number of particles
        case KEY_KP_1:
        case KEY_KP_2:
        case KEY_KP_3:
        case KEY_KP_4:
        case KEY_KP_5:
        case KEY_KP_6:
        case KEY_KP_7:
        case KEY_KP_8:
        case KEY_KP_9:
            _particleManager.init(presets[key - KeyboardKey::KEY_KP_1]);
            clearHistory(0);
            _nextCmdIndex = 0;
            _particleManager.setDefaultColor();
            break;

        case KEY_Z:
        {
            if (IsKeyDown(KEY_LEFT_CONTROL))
            {
                IsKeyDown(KEY_LEFT_SHIFT)? redo(): undo();
            }
        } break;
        default:
            break;
        }
    }

    void GameSPH::update()
    {
        if (_pause)
            return;

        _particleManager.update();
    }

    void GameSPH::render()
    {
        BeginDrawing();
        {
            // Clear screen
            ClearBackground(Color{ 220, 220, 220, 255 });

            // Draw particles
            _particleManager.render();

            DrawFPS(20, 20);
        }
        EndDrawing();
    }

    void GameSPH::addCommand(ICommand* cmd)
    {
        cmd->execute();
        
        // Flush all entries after last executed cmd
        clearHistory(_nextCmdIndex);
        _cmdHistory.push_back(cmd);
        ++_nextCmdIndex;

        cout << "command added, _nextCmdIndex = " << _nextCmdIndex << endl;
    }

    void GameSPH::undo()
    {
        if (_nextCmdIndex == 0)
            return;
        
        --_nextCmdIndex;
        _cmdHistory[_nextCmdIndex]->undo();
        cout << "undo, _nextCmdIndex = " << _nextCmdIndex << endl;
    }

    void GameSPH::redo()
    {
        if (_nextCmdIndex >= _cmdHistory.size())
            return;

        _cmdHistory[_nextCmdIndex]->execute();
        ++_nextCmdIndex;
        cout << "redo, _nextCmdIndex = " << _nextCmdIndex << endl;
    }

    void GameSPH::clearHistory(uint index)
    {
        // Prevent memory leak
        for (uint i{ index }; i < _cmdHistory.size(); ++i)
            delete _cmdHistory[i];

        
        _cmdHistory.resize(index);
    }
}
