#include "ParticleManager.h"

#include <raylib.h>
#include <Code_Utilities_Light_v2.h>

#include "Globals.h"

using namespace SPH;

Particle::Particle(double px, double py) 
    : x(px), y(py)
    , vx{}, vy{}
    , fx{}, fy{}
    , rho{}, p{}
{}

ParticleManager::ParticleManager()
{
    _ax = 0;
    _ay = GRAVITY;

    _renderMode = (uchar)Render::Particles;
    BdB::srandInt((uint)time(0));
}

void ParticleManager::init(ulong n)
{
    cout << "Init with " << n << " particles" << endl;

    _particles.clear();
    _particles.reserve(n);

    while (_particles.size() < n)
    {
        double x = BdB::randInt(SCREEN_WIDTH);
        double y = BdB::randInt(SCREEN_HEIGHT);

        double tmpX = x - SCREEN_WIDTH * 0.5;
        double tmpY = y - SCREEN_HEIGHT * 0.5;
        double centerDistSqrt = tmpX * tmpX + tmpY * tmpY;

        double tmpRef = fmin(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.25;
        if (centerDistSqrt < tmpRef * tmpRef)
            _particles.push_back(Particle(x, y));
    }
}

int ParticleManager::addBlock(int center_x, int center_y)
{
    int particleAdded = 0;
    for (int i=0; i<=4; ++i) 
        for (int j=0; j<=4; ++j)
        {
            double x = center_x + (j - 2) * SCREEN_WIDTH * 0.04f + BdB::randInt((int)H);
            double y = center_y + (i - 2) * SCREEN_HEIGHT * 0.04f + BdB::randInt((int)H);

            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                _particles.push_back(Particle(x, y));
                ++particleAdded;
            }
        }

    cout << _particles.size() << " particles" << endl;
    return particleAdded;
}

const Color& ParticleManager::getColor() const
{
    return _color;
}

void ParticleManager::changeColor(uchar r, uchar g, uchar b)
{
    _color.r = r;
    _color.g = g;
    _color.b = b;
}

void ParticleManager::setDefaultColor()
{
    _color = defaultColor;
}

void ParticleManager::removeParticles(uint nb)
{
    size_t currentSize = _particles.size();
    if (currentSize > nb)
        _particles.resize(currentSize - nb);
    else
        _particles.clear();

    cout << _particles.size() << " particles" << endl;
}

void ParticleManager::addOne(int x, int y)
{
    _particles.push_back(Particle(x, y));
    cout << _particles.size() << " particles" << endl;
}

void ParticleManager::setGravity(int direction)
{
    switch (direction) 
    {
    case DOWN:
        _ax = 0;
        _ay = +GRAVITY;
        break;
    case UP:
        _ax = 0;
        _ay = -GRAVITY;
        break;
    case RIGHT:
        _ax = +GRAVITY;
        _ay = 0;
        break;
    default:
        _ax = -GRAVITY;
        _ay = 0;
    }
}

void ParticleManager::explode() 
{
    for (auto &p : _particles) 
    {
        p.vx = BdB::randInt(-5000, 5000);
        p.vy = BdB::randInt(-5000, 5000);
    }
}

void SPH::ParticleManager::clearGrid()
{
    for (ColumnList& col: _grid)
        for (RefList& entry: col)
            entry.clear();
}

void SPH::ParticleManager::feedGrid()
{
    clearGrid();

    for (Particle& p : _particles)
        _grid[refX(p)][refY(p)].push_back(&p);
}

uint SPH::ParticleManager::refX(const Particle& p)
{
    return (static_cast<uint>(p.x) >> BDH) % (ROW_SIZE);
}

uint SPH::ParticleManager::refY(const Particle& p)
{
    return (static_cast<uint>(p.y) >> BDH) % (COL_SIZE);
}

void ParticleManager::integrate(double dt)
{
    for (auto &p : _particles)
    {
        // forward Euler integration
        if (p.rho != 0 && p.fx == p.fx && p.fy == p.fy) 
        {
            p.vx += dt*p.fx/p.rho;
            p.vy += dt*p.fy/p.rho;
        }

        p.x += dt*p.vx;
        p.y += dt*p.vy;

        // enforce boundary conditions
        if (p.x - PARTICLE_RADIUS < 0.0f)
        {
            p.vx *= BOUND_DAMPING;
            p.x = PARTICLE_RADIUS;
        }

        if (p.x + PARTICLE_RADIUS > SCREEN_WIDTH)
        {
            p.vx *= BOUND_DAMPING;
            p.x = SCREEN_WIDTH - PARTICLE_RADIUS;
        }

        if (p.y - PARTICLE_RADIUS < 0.0f)
        {
            p.vy *= BOUND_DAMPING;
            p.y = PARTICLE_RADIUS;
        }

        if (p.y + PARTICLE_RADIUS > SCREEN_HEIGHT)
        {
            p.vy *= BOUND_DAMPING;
            p.y = SCREEN_HEIGHT - PARTICLE_RADIUS;
        }
    }
}

void ParticleManager::computeDensityPressure()
{
    // Pour chaque particule
    for (auto &pi : _particles)
    {
        pi.rho = 0.f;

        // Chercher toutes les particules qui contribuent à la
        // pression/densité
        int coordX = refX(pi);
        int coordY = refY(pi);

        // process 9 positions near a particle
        for (int x{ -1 }; x <= 1; ++x)
        {
            int nearX = coordX + x;
            if (nearX < 0 || nearX >= ROW_SIZE )
                continue;

            for (int y{-1}; y<=1; ++y)
            {
                int nearY = coordY + y;
                if (nearY < 0 || nearY >= COL_SIZE)
                    continue;

                for (Particle* pj : _grid[nearX][nearY])
                {
                    double tempX = pj->x - pi.x;
                    double tempY = pj->y - pi.y;
                    double distanceSqrt = tempX * tempX + tempY * tempY;

                    if (distanceSqrt < HSQ)
                    {
                        // this computation is symmetric
                        double tmpProcess = HSQ - distanceSqrt;
                        pi.rho += MASS_POLY6 * tmpProcess * tmpProcess * tmpProcess;
                    }
                }
            }
        }
            
        pi.p = GAS_CONST*(pi.rho - REST_DENS);
    }
}

void ParticleManager::computeForces()
{
    // Pour chaque particule
    for (auto &pi : _particles) 
    {
        double pressure_x = {};
        double pressure_y = {};

        double viscosity_x = {};
        double viscosity_y = {};

        int coordX = refX(pi);
        int coordY = refY(pi);
        
        // process 9 positions near a particle
        for (int x{ -1 }; x <= 1; ++x)
        {
            int nearX = coordX + x;
            if (nearX < 0 || nearX >= ROW_SIZE)
                continue;

            for (int y{ -1 }; y <= 1; ++y)
            {
                int nearY = coordY + y;
                if (nearY < 0 || nearY >= COL_SIZE)
                    continue;

                // Calculer la somme des forces de viscosité et pression appliquées par les autres particules
                for (Particle* pj : _grid[nearX][nearY])
                {
                    if (&pi == pj)
                        continue;

                    double tmpX = pj->x - pi.x;
                    double tmpY = pj->y - pi.y;
                    double rSqrt = tmpX * tmpX + tmpY * tmpY;

                    if (rSqrt < HSQ)
                    {
                        double r = sqrt(rSqrt);

                        // compute pressure force contribution
                        double tmpProcess = H - r;
                        double fpress = MASS_SPIKY_GRAD * (pi.p + pj->p) / (2.0 * pj->rho) * tmpProcess * tmpProcess;
                        pressure_x += (pi.x - pj->x) / r * fpress;
                        pressure_y += (pi.y - pj->y) / r * fpress;

                        // compute viscosity force contribution
                        viscosity_x += MASS_VISC_LAP * (pj->vx - pi.vx) / pj->rho * (H-r);
                        viscosity_y += MASS_VISC_LAP * (pj->vy - pi.vy) / pj->rho * (H-r);
                    }
                }
            }
        }

        pi.fx = pressure_x + viscosity_x + _ax * pi.rho;
        pi.fy = pressure_y + viscosity_y + _ay * pi.rho;
    }
}

void ParticleManager::update()
{
    float dt = GetFrameTime();

    feedGrid();

    computeDensityPressure();
    computeForces();
    integrate(dt/10);
}

void ParticleManager::setRenderMode(uchar mask)
{
    _renderMode = mask;
}

void ParticleManager::renderParticles() 
{
    Rectangle r{};

    // Draw particles
    for (long unsigned int i=0; i<_particles.size(); i++) 
    {
        r.x = static_cast<float>(_particles[i].x - PARTICLE_RADIUS);
        r.y = static_cast<float>(_particles[i].y - PARTICLE_RADIUS);
        r.width  = static_cast<float>(PARTICLE_RADIUS * 2);
        r.height = static_cast<float>(PARTICLE_RADIUS * 2);
        DrawRectangleRec(r, _color);
    }
}

void ParticleManager::renderGrid() 
{
    for (uint i{}; i < ROW_SIZE; ++i)
    {
        uint posX = CEll_SIZE * i;
        DrawLine(posX, 0, posX, SCREEN_HEIGHT, LIGHTGRAY);
    }

    for (uint i{}; i < COL_SIZE; ++i)
    {
        uint posY = CEll_SIZE * i;
        DrawLine(0, posY, SCREEN_WIDTH, posY, LIGHTGRAY);
    }
}

void ParticleManager::renderCells() 
{
    Color c{ 0, 0, 255 };
    Rectangle r{};

    for (uint i{}; i < NB_CELLS; ++i)
    {
        uint x = i % ROW_SIZE;
        uint y = i / ROW_SIZE;

        r.x = static_cast<float>(x * CEll_SIZE);
        r.y = static_cast<float>(y * CEll_SIZE);
        r.width = static_cast<float>(CEll_SIZE);
        r.height = static_cast<float>(CEll_SIZE);


        c.a = (_grid[x][y].size() * ALPHA_RATIO) % 256;

        if (c.a > 0)
            DrawRectangleRec(r, c);
    }
}

void ParticleManager::render()
{
    if (_renderMode & (uchar)Render::Particles)
        renderParticles();

    if (_renderMode & (uchar)Render::DrawGrid)
    {
        renderCells();
        renderGrid();
    }
}
