#include "Commands.h"
#include "ParticleManager.h"
#include <Raylib.h>

namespace SPH
{
    ICommand::ICommand(ParticleManager& pm)
        : _pm{pm}
    {}

    CmdAddOne::CmdAddOne(ParticleManager& pm, int x, int y)
        : ICommand{pm}
        , _x{x}, _y{y}
    {
    }

    void CmdAddOne::execute()
    {
        _pm.addOne(_x,_y);
    }

    void CmdAddOne::undo()
    {
        _pm.removeParticles(1);
    }

    CmdAddGroup::CmdAddGroup(ParticleManager& pm, int x, int y)
        : ICommand{pm}
        , _x{ x }, _y{ y }
        , _nbParticles{}
    {}

    void CmdAddGroup::execute()
    {
        _nbParticles = _pm.addBlock(_x, _y);
    }

    void CmdAddGroup::undo()
    {
        _pm.removeParticles(_nbParticles);
    }

    CmdChangeColor::CmdChangeColor(ParticleManager& pm, const Color& old)
        : ICommand{ pm }
        , _or{ old.r}, _og{ old.g}, _ob{ old.b }
        , _r{}, _g{}, _b{}
    {}

    void CmdChangeColor::setNew(uchar r, uchar g, uchar b)
    {
        _r = r;
        _g = g;
        _b = b;
    }

    void CmdChangeColor::execute()
    {
        _pm.changeColor(_r, _g, _b);
    }

    void CmdChangeColor::undo()
    {
        _pm.changeColor(_or, _og, _ob);
    }
}