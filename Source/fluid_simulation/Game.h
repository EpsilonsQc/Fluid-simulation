#pragma once

namespace Core
{
    class Game
    {
    public:
        virtual ~Game();

        void loop();

        virtual void handleInput() = 0;
        virtual void update() = 0;
        virtual void render() = 0;

    protected:
        Game();
        bool _keepPlaying;
    };
}
