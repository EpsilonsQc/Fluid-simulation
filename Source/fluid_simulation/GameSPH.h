#pragma once

#include <array>

#include "Game.h"
#include "ParticleManager.h"
#include "Globals.h"

using namespace Core;

namespace SPH
{
    class ICommand;
    class GameSPH final : public Game 
    {
        enum {PRESET=9};
        using PresetList = std::array<ulong, PRESET>;
        using CommandList = std::vector<ICommand*>;

    public:
        GameSPH();
        ~GameSPH();

        virtual void handleInput() override;
        virtual void update() override;
        virtual void render() override;

    private:
        inline static const uint FPS = 30;

        bool _pause;
        inline static PresetList presets = {1, 200, 400, 700, 900, 1500, 2000, 3000, 5000};
        ParticleManager _particleManager;

        int getClickX();
        int getClickY();

    private:
        uint _nextCmdIndex;
        CommandList _cmdHistory;

        void addCommand(ICommand* cmd);
        void undo();
        void redo();
        void clearHistory(uint index);
    };
}
