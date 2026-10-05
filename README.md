# N-body Gravitational Simulation

A 2D particle simulation written in C++17, with SFML for rendering. Particles interact through Newtonian gravity and collide as circular bodies.

I built this project to explore numerical simulation and develop my C++ skills, particularly class design, memory management and collision detection.

## Features

- Pairwise gravitational forces between particles.
- Fixed physics timestep, with interpolated positions for rendering between updates.
- Continuous collision detection using predicted contact times.
- Collision response based on conservation of momentum and a coefficient of restitution.
- Overlap correction and numerical tolerances for handling particles near contact.
- Separate classes for particle data, simulation logic and rendering.

## How it works

### Gravity and motion

Each particle stores its mass, radius, position, velocity and acceleration. The gravitational force on particle $i$ due to particle $j$ is

$$
\mathbf{F}_{ij} = G\frac{m_i m_j}{|\mathbf{r}_{ij}|^3}\mathbf{r}_{ij},
$$

where $\mathbf{r}_{ij}$ points from particle $i$ to particle $j$. The net force determines its acceleration, which is used to update its velocity and position.

The simulation uses a fixed timestep. An accumulator separates physics updates from rendering, and the visualiser interpolates between the previous and current positions to smooth motion between updates.

### Collisions

The collision detector estimates when two particles will touch by solving

$$
|\mathbf{r} + \mathbf{v}t|^2 = (r_1 + r_2)^2,
$$

where $\mathbf{r}$ and $\mathbf{v}$ are their relative position and velocity. This gives a quadratic equation for the contact time.

The simulation searches for the earliest contact within the remaining timestep, advances the particles, resolves the collision and repeats for the time left. Collision response changes the velocity components along the line joining the particle centres. A coefficient of restitution controls how much the particles rebound.

Existing overlaps are handled separately through position correction. Numerical tolerances help avoid repeatedly resolving negligible contact errors.

## Code structure

| Class | Responsibility |
| --- | --- |
| `Particle` | Stores particle properties and updates acceleration, velocity and position. |
| `Simulation` | Owns the particles and handles gravity, collision detection and collision response. |
| `Visualiser` | Draws particles using SFML and interpolates their positions for rendering. |

Particles are owned by `Simulation` through a vector of `std::unique_ptr<Particle>`.

## Building and running

The project uses **C++17**, **SFML 3** and **CMake**. Development has been on Windows with MinGW-w64 and Ninja.

With the compiler, CMake, Ninja and the required SFML dependencies available, run these commands from the repository root:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target n_body_simulation
```

Run the generated `n_body_simulation` executable (`n_body_simulation.exe` on Windows) from the runtime output directory configured in `CMakeLists.txt`. When using dynamically linked SFML on Windows, its runtime DLLs must be available beside the executable or on `PATH`.

## Changing the simulation

Initial conditions and simulation parameters are set in the source code. These include particle masses, starting positions and velocities, gravitational strength, timestep, and the mass-to-radius relationship. Rebuild after changing them.

The parameters use simulation units and can be adjusted to explore different behaviours.

## Limitations

This is a learning project and is still being developed.

- Gravitational force calculation scales as $O(N^2)$. Collision searches also check particle pairs and may repeat several times within one timestep.
- Collision-time prediction assumes constant relative velocity, while gravity changes particle velocities. Close encounters therefore remain sensitive to the timestep.
- Dense groups, deep overlaps and repeated contacts are challenging cases for the collision solver.
- Numerical integration introduces error; exact long-term energy conservation is not guaranteed.

## Possible improvements

- Add momentum and energy diagnostics to measure numerical error.
- Test isolated two-body orbits and simple collision cases against analytical results.
- Improve handling of simultaneous collisions and persistent contact.
- Add spatial partitioning to reduce the number of collision checks.
