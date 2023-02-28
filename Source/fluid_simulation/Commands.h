#pragma once
#include "Globals.h"

struct Color;
namespace SPH
{
    class ParticleManager;
    class ICommand
    {
    protected: 
        ParticleManager& _pm;
        ICommand(ParticleManager&);
    public:
        virtual ~ICommand() = default;
    
        virtual void execute() = 0;
        virtual void undo() = 0;
    
    };

    class CmdAddOne : public ICommand
    {
        int _x, _y;
    public:
        CmdAddOne(ParticleManager&, int, int);
        void execute() override;
        void undo() override;
    };

    class CmdAddGroup : public ICommand
    {
        int _x, _y;
        int _nbParticles;
    public:
        CmdAddGroup(ParticleManager&, int, int);
        void execute() override;
        void undo() override;
    };

    class CmdChangeColor : public ICommand
    {
        uchar _r, _g, _b;
        uchar _or, _og, _ob;

    public:
        CmdChangeColor(ParticleManager&, const Color&);
        void setNew(uchar, uchar, uchar);
        void execute() override;
        void undo() override;
    };
}