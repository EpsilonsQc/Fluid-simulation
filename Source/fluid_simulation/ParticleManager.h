#pragma once

// Smoothed-particle hydrodynamics simulation, based on Matthias Müller paper
// https://matthias-research.github.io/pages/publications/sca03.pdf

// The code is derived from implementation under MIT license by Lucas V. Schuermann
// https://github.com/cerrno/mueller-sph/tree/d24d025ce496db89de62ad4359bf89b175c712ed

// Writeup
// https://lucasschuermann.com/writing/implementing-sph-in-2d

#include <vector>
#include <array>
#include <string>
#include <raylib.h>

#include "Globals.h"

namespace SPH
{
    // Particle data structure
    struct Particle
    {
        Particle() = default;
        Particle(double, double);
        double x, y;   // Position
        double vx, vy; // Velocity
        double fx, fy; // Total forces
        double rho;    // Density
        double p;      // Pressure
    };

    enum class Render
    {
        Particles   = 1 << 0,
        DrawGrid    = 1 << 1
    };

    class ParticleManager
    {
        using cint = const int;
        using cdouble = const double;

        inline static cdouble H = 16.0; // kernel radius
        inline static cint BDH = 4; // kernel radius
        inline static cint CEll_SIZE = static_cast<cint>(H);
        inline static cint ROW_SIZE = SCREEN_WIDTH >> BDH;
        inline static cint COL_SIZE = SCREEN_HEIGHT >> BDH;
        inline static cint NB_CELLS = ROW_SIZE * COL_SIZE;

        const Color defaultColor{ 230, 120, 0, 100 };
        inline static cint ALPHA_LV = 5;
        inline static cint ALPHA_RATIO = 255 / ALPHA_LV;

        using RefList = std::vector<Particle*>;

        using ColumnList = std::array<RefList, COL_SIZE>;
        using ParticleGrid = std::array<ColumnList, ROW_SIZE>;

    public:
        inline static cdouble REST_DENS = 200.0; // rest density
        inline static cdouble GAS_CONST = 200.0; // const for equation of state
        inline static cdouble HSQ = H*H; // radius^2 for optimization
        inline static cdouble MASS = 65.0; // assume all particles have the same mass
        inline static cdouble VISC = 25.0; // viscosity constant
        inline static cdouble GRAVITY = 20000;
       

        // size of a particle
        inline static cdouble PARTICLE_RADIUS = H / 4.0;

        // smoothing kernels defined in Müller and their gradients
        inline static cdouble POLY6 = 315.0 / (65.0 * PI * pow(H, 9.0));
        inline static cdouble SPIKY_GRAD = -45.0 / (PI * pow(H, 6.0));
        inline static cdouble VISC_LAP = 45.0 / (PI * pow(H, 6.0));

        // Pre process constant
        inline static cdouble MASS_POLY6 = MASS * POLY6;
        inline static cdouble MASS_SPIKY_GRAD = MASS * SPIKY_GRAD;
        inline static cdouble MASS_VISC_LAP = MASS * VISC * VISC_LAP;

        // simulation parameters
        inline static cdouble BOUND_DAMPING = -0.9;

        ParticleManager();

        void init(ulong);
        void addOne(int, int);
        int addBlock(int, int);
        const Color& getColor() const;
        void changeColor(uchar, uchar, uchar);
        void setDefaultColor();
        void removeParticles(uint);
        void setGravity(int);
        void explode();

        void update();
        void render();

        void setRenderMode(uchar);

    private:
        double _ax, _ay; // Gravity

        void clearGrid();
        void feedGrid();
        uint refX(const Particle&);
        uint refY(const Particle&);

        void integrate(double dt);

        void computeDensityPressure();
        void computeForces();
        std::vector<Particle> _particles;
        Color _color{ defaultColor};

        uchar _renderMode;
        void renderParticles();

        void renderGrid();
        void renderCells();

        ParticleGrid _grid;
    };
}
