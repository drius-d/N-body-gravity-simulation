#include "simulation.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <cassert>

Simulation::Simulation(const int n, const double dt)
: numberOfParticles(n), dt(dt) {

    // Loop runs for the number of particles selected and creates particles

    for (int i = 1; i <= n; i++){
        double mass;
        std::array<double, 2> position;      
        std::array<double, 2> velocity;       
        std::cout << "Specify particle's mass, position, velocity (optional).\n";
        std::cout << "Input 3-5 floats (i.e. 50.0 0.0 2.0 1.0 3.0 or 50.0 0.0 2.0): ";

        // Checks if the input is a recognised format to 
    
        bool noVelocityInput = false;
        while (true){

            // Using istringstream instead because it fails
            // when it tries to read the buffer while its empty unlike cin.

            std::string input;
            std::getline(std::cin, input);

            std::istringstream stream(input);

            // If can't read from buffer into mass and position, clears buffer and 
            // forces user to input again.

            if (!(stream >> mass >> position[0] >> position[1])){
                stream.clear();
                stream.str("");

                std::cout << "Please try again: ";
                continue;
            }

            // If can't read from buffer into velocity, clears buffer and moves on
            // since velocity is not required

            if (!(stream >> velocity[0] >> velocity[1])){
                stream.clear();
                stream.str("");
                
                // lets simulation know whether it needs to pass velocity as an argument into the Particle constructor
                noVelocityInput = true;     
            }

            // In case buffer contains >6 entries, 

            stream.str("");

            break;
        }

        if (noVelocityInput){
            std::unique_ptr<Particle> p = std::make_unique<Particle>(mass, position);
            particles.push_back(std::move(p));
        }
        else {
            std::unique_ptr<Particle> p = std::make_unique<Particle>(mass, position, velocity);
            particles.push_back(std::move(p));
        }
    }

    for (const std::unique_ptr<Particle>& p : particles){
        previousPositions.push_back(p->getPosition());
    }
}
    
void Simulation::updateSimulation(){
    
    // update RelativePositionsMatrix
    relativePositionsMatrix.clear();
    calculateRelativePositions();

    // update previous positions of all particles to the old current positions
    previousPositions.clear();
    storePreviousPositions();

    // using RelativePositionsMatrix calculate force and update each particle's acceleration
    forceUpdater();

    // processing any collisions between time-steps
    processCollisions();

}

void Simulation::calculateRelativePositions(){

    for (int i = 0; i < numberOfParticles; i++){

        // defines a vector containing the i-th particle's relative positions with all particles
        // (including its relative position with itself i.e. (0,0).

        std::vector<std::array<double, 2>> relativePositions;
        const std::array<double, 2> ownPosition = (*particles[i]).getPosition();

        for (int j = 0; j < numberOfParticles; j++){

            // calculates relative position between i-th particle and all particles in scope
            // and pushes it to relativePositions

            const std::array<double, 2> otherPosition = (*particles[j]).getPosition();
            std::array<double, 2> relativePosition;
            
            relativePosition[0] = otherPosition[0] - ownPosition[0];
            relativePosition[1] = otherPosition[1] - ownPosition[1];

            relativePositions.push_back(relativePosition);
        }

        // creates matrix of positions. see include/simulation.hpp comments in class Simulation
        // for an idea.

        relativePositionsMatrix.push_back(relativePositions);
    }
}

void Simulation::storePreviousPositions(){
    for (const std::unique_ptr<Particle>& p : particles){
        previousPositions.push_back(p->getPosition());
    }
}

void Simulation::processCollisions(){
    double remainingTime = dt;

    // defensive iteration cap

    int iteractionCount = 0;
    constexpr int MAX_ITERATIONS = 10000;

    // loop through all collisions 

    while (remainingTime > 0){

        if (++iteractionCount > MAX_ITERATIONS){

            std::cout << "Remaining time: " << remainingTime << "\n";
            break;
        }

        bool overlapResolved = false;
            
        // earliestCollisionTime implemented to check when the updater should advance to

        double earliestCollisionTime = remainingTime;

        // to store relevant collision particles

        int collisionParticle1 = -1;
        int collisionParticle2 = -1;

        for (int i = 0; i < particles.size(); i++){

            // obtains particle i with its position and velocity

            auto& currentParticle = particles[i];
            std::array<double, 2>& currentParticlePosition = currentParticle->getPosition();
            std::array<double, 2>& currentParticleVelocity = currentParticle->getVelocity();

            for (int j = i + 1; j < particles.size(); j++){ 

                // obtain particle i+1 with its position and velocity
                auto& otherParticle = particles[j];
                std::array<double, 2>& otherParticlePosition = otherParticle->getPosition();
                std::array<double, 2>& otherParticleVelocity = otherParticle->getVelocity();

                // R is the sum of the radii of the two particles
                double R = currentParticle->getRadius() + otherParticle->getRadius();

                const double SLOP = R * 1e-6;

                // used for CCD scheduling to prevent a small upwards rounding error from
                // triggering a collision deteection
                double R_eff = R - SLOP;

                // r is the distance between the centre of the particles
                std::array<double, 2> r;
                r[0] = otherParticlePosition[0] - currentParticlePosition[0];
                r[1] = otherParticlePosition[1] - currentParticlePosition[1];
                
                // v is the relative velocity of the two particles
                std::array<double, 2> v;
                v[0] = otherParticleVelocity[0] - currentParticleVelocity[0];
                v[1] = otherParticleVelocity[1] - currentParticleVelocity[1];

                double distance = std::sqrt(r[0]*r[0] + r[1]*r[1]);

                // old code detected penetration against R_eff and then corrected 
                // position to R_eff. but any small rounding error could flip the position
                // back into the overlap zone. the code still reruns again because it thinks
                // its just corrected an overlap but it hasn't. it detects the exact same overlap
                // it tried to correct but this time at a tiny penetration. it tries to do a correction
                // smaller than the penetration which at certain distances can register as a zero 
                // correction leading to infinite loop. 
                // to fix this we detected penetration against actual R and then corrected to
                // R + SLOP/2 to prevent rounding us back into overlap.
                double truePenetration = R - distance;

                // checks if particles already overlapping beyond the allowed slop
                if (truePenetration > SLOP){

                    // change this later

                    if (distance == 0){     
                        continue;
                    }

                    // calculating normal unit vector
                    std::array<double, 2> normal;
                    normal[0] = r[0] / distance;
                    normal[1] = r[1] / distance;

                    // retrieving velocities
                    std::array<double, 2> vel1 = currentParticle->getVelocity();
                    std::array<double, 2> vel2 = otherParticle->getVelocity();

                    // retrieving masses

                    double m1 = currentParticle->getMass();
                    double m2 = otherParticle->getMass();

                    // getting u1 and u2

                    double u1 = std::inner_product(vel1.begin(), vel1.end(), normal.begin(), 0.0);
                    double u2 = std::inner_product(vel2.begin(), vel2.end(), normal.begin(), 0.0);

                    // working out velocity epsilons
                    // these are useful to prevent small rounding errors in velocity from triggering a kick
                    // floating point floor: epsilon_fp = k_fp * u * v_esc, where u = 2.22e-16 (machine epsilon for double) and v_esc is the escape velocity at contact

                    double k_fp = 50;
                    double u = 2.22e-16;
                    double v_esc = std::sqrt((2 * G * (m1 + m2)) / R);

                    double epsilon_fp = k_fp * u * v_esc;

                    // gravity's own per step kick: epsilon_g = k_grav * a_contact * dt

                    double k_grav = 3;
                    double a_contact = (G * (m1 + m2)) / (R * R);
                    
                    double epsilon_g = k_grav * a_contact * remainingTime;

                    // physical floor: small fraction of escape velocity epsilon_phys = f * v_esc

                    double f = 1e-3;
                    double epsilon_phys = f * v_esc;

                    double epsilon = std::max({epsilon_fp, epsilon_g, epsilon_phys});

                    // resolving overlap by moving the particles apart, weighted by mass of particle.
                    // target leaving HALF a slop's worth of residual penetration rather than zero

                    double correctionAmount = truePenetration - (SLOP / 2.0);

                    if (correctionAmount > 0){
                        double currentPenetration = (m2 * correctionAmount) / (m1 + m2);
                        double otherPenetration = (m1 * correctionAmount) / (m1 + m2);

                        currentParticlePosition[0] -= normal[0] * currentPenetration;
                        currentParticlePosition[1] -= normal[1] * currentPenetration;

                        otherParticlePosition[0] += normal[0] * otherPenetration;
                        otherParticlePosition[1] += normal[1] * otherPenetration;
                    }

                    // epsilon gate here to differentiate between a "resting contact velocity"
                    // and a genuine collision velocity
                    if (u1 - u2 > epsilon){
                        double c1 = m1*u1 + m2*u2;
                        double c2 = e * (u1 - u2);

                        std::array<double, 2> matrixValues = solve2x2Matrix(m1, m2, -1.0, 1.0, c1, c2);

                        double v1 = matrixValues[0];
                        double v2 = matrixValues[1];

                        currentParticleVelocity[0] += (v1 - u1) * normal[0];
                        currentParticleVelocity[1] += (v1 - u1) * normal[1];

                        otherParticleVelocity[0] += (v2 - u2) * normal[0];
                        otherParticleVelocity[1] += (v2 - u2) * normal[1];
                    }

                    // if its not registered as a genuine collision velocity we provide a smaller kick
                    // not a full restitution bounce
                    else if (u1 - u2 > 0){

                        double c1 = m1*u1 + m2*u2;
                        double c2 = 0.0;

                        std::array<double, 2> matrixValues = solve2x2Matrix(m1, m2, -1.0, 1.0, c1, c2);

                        double v1 = matrixValues[0];
                        double v2 = matrixValues[1];

                        currentParticleVelocity[0] += (v1 - u1) * normal[0];
                        currentParticleVelocity[1] += (v1 - u1) * normal[1];

                        otherParticleVelocity[0] += (v2 - u2) * normal[0];
                        otherParticleVelocity[1] += (v2 - u2) * normal[1];
                    }

                    overlapResolved = true;

                    break;
                }

                // some complicated maths: basically using some geometry you can determine that
                // the equation for the time of collision for any two particles is given by a quadratic
                // at^2 + bt + c = 0, where a is dot product of v and v, b is 2 * dot product of r and v,
                // c is dot product of r and r - R^2

                double a = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
                double b = 2 * std::inner_product(r.begin(), r.end(), v.begin(), 0.0);
                double c = std::inner_product(r.begin(), r.end(), r.begin(), 0.0) - R_eff * R_eff;
                
                double discriminant = b*b - 4 * a * c;

                // checks for any illegal maths i.e. division by 0 and imaginary numbers
                if (a == 0){
                    continue;
                }

                if (discriminant < 0){
                    continue;
                }

                // two possible collision times because quadratic. need to check both
                double collisionTime1 = (- b + std::sqrt(discriminant)) / (2 * a);
                double collisionTime2 = (- b - std::sqrt(discriminant)) / (2 * a);

                // checks if time greater than 0, less than time until next update, and if its less than an already saved earliestTime
                // stores earliest time if found and the involved collision particles
                if (collisionTime1 > 0 && collisionTime1 < remainingTime &&
                    collisionTime1 < earliestCollisionTime){
                
                    earliestCollisionTime = collisionTime1;
                    collisionParticle1 = i;
                    collisionParticle2 = j;
                }

                if (collisionTime2 > 0 && collisionTime2 < remainingTime &&
                    collisionTime2 < earliestCollisionTime){
                
                    earliestCollisionTime = collisionTime2;
                    collisionParticle1 = i;
                    collisionParticle2 = j;
                }
            }

            // overlap found so we need to restart the process and stop searching other pairs
            if (overlapResolved){
                break;
            }
        }

        if (overlapResolved){
            // particles have changed positions so we should start from scratch

            continue;
        }

        // if no collision then update to remaining time and exit loop
        if (collisionParticle1 == -1){
            for (auto& particle : particles){
                particle->updateVelocity(remainingTime);
                particle->updatePosition(remainingTime);
            }

            remainingTime = 0;
            continue;
        }

        // otherwise update to earliest collision time
        for (int i = 0; i < particles.size(); i++){
            auto& p = particles[i];
            p->updateVelocity(earliestCollisionTime);
            p->updatePosition(earliestCollisionTime);
        }   

        // obtain relevant particle positions and relative position
        Particle* pParticle = particles[collisionParticle1].get();
        Particle* pOther = particles[collisionParticle2].get();

        std::array<double, 2>& p1Position = pParticle->getPosition();
        std::array<double, 2>& p2Position = pOther->getPosition();
        
        std::array<double, 2> relPosition;
        relPosition[0] = p2Position[0] - p1Position[0];
        relPosition[1] = p2Position[1] - p1Position[1];

        // calculates distance between centres
        double distance = std::sqrt(relPosition[0] * relPosition[0] + relPosition[1] * relPosition[1]);

        // only performs collision resolution if distance non-zero

        if (distance != 0){
            
            // calculates normal pointing between particle centres

            std::array<double, 2> normal;
            normal[0] = relPosition[0] / distance;
            normal[1] = relPosition[1] / distance;

            // gets particle masses and velocities

            double m1 = pParticle->getMass();
            double m2 = pOther->getMass();
            std::array<double, 2>& currentParticleVelocity = pParticle->getVelocity();
            std::array<double, 2>& otherParticleVelocity = pOther->getVelocity();

            // calculates component of velocity for both particles along normal using inner product

            double u1 = std::inner_product(currentParticleVelocity.begin(), currentParticleVelocity.end(), normal.begin(), 0.0);
            double u2 = std::inner_product(otherParticleVelocity.begin(), otherParticleVelocity.end(), normal.begin(), 0.0);

            // simultaneous equations consist of conservation of momentum and coefficient of restitution formula
            // c1 and c2 are to simplify the numerical values on the RHS

            double c1 = m1*u1 + m2*u2;
            double c2 = e * (u1 - u2);

            // m1*v1 + m2*v2 = c1
            // v2 - v1 = c2

            // uses Cramer's rule to solve 2x2 matrix for v1 and v2 

            std::array<double, 2> matrixValues = solve2x2Matrix(m1, m2, -1.0, 1.0, c1, c2);

            double v1 = matrixValues[0];
            double v2 = matrixValues[1];

            // updates the particle velocities with the change in velocity along the line of collision

            currentParticleVelocity[0] += (v1 - u1) * normal[0];
            currentParticleVelocity[1] += (v1 - u1) * normal[1];
            
            otherParticleVelocity[0] += (v2 - u2) * normal[0];
            otherParticleVelocity[1] += (v2 - u2) * normal[1];

        }
        remainingTime -= earliestCollisionTime;
    }
}

void Simulation::forceUpdater(){

    // F = G Mm/r^2 or in component form:
    // Fx = (k * x) / (x^2 + y^2)^(3/2), Fy = (k * y) / (x^2 + y^2)^(3/2)
    // For each particle, calculate the force vectors, calculate a net force, then
    // call particle update method 

    for (int i = 0; i < numberOfParticles; i++){
        Particle& currentParticle = (*particles[i]);
        std::array<double, 2> netForce = {0, 0};
        double massI = currentParticle.getMass();
        
        for (int j = 0; j < numberOfParticles; j++){
            if (i == j){
                continue;
            }

            const Particle& otherParticle = (*particles[j]);
            double massJ = otherParticle.getMass();
            double dx = relativePositionsMatrix[i][j][0];
            double dy = relativePositionsMatrix[i][j][1];

            double Fx = (G * massI * massJ * dx) / std::pow((dx*dx + dy*dy), 1.5);
            double Fy = (G * massI * massJ * dy) / std::pow((dx*dx + dy*dy), 1.5);

            netForce[0] += Fx;
            netForce[1] += Fy;
        }

        currentParticle.updateAcceleration(netForce);
    }
}

void Simulation::advanceParticles(double time){
    for (auto& particle: particles){
        particle->updatePosition(time);
        particle->updateVelocity(time);
    }
}

const std::vector<std::unique_ptr<Particle>>& Simulation::returnParticles() const{
    return particles;
}

const int Simulation::returnNumberOfParticles() const{
    return numberOfParticles;
}

const std::vector<std::array<double, 2>>& Simulation::returnPreviousPositions() const{
    return previousPositions;
}

void Simulation::printMatrix() const {
    std::cout << "{\n";
    for (const std::vector<std::array<double, 2>>& row : relativePositionsMatrix){
        std::cout << "{ ";
        for (const std::array<double, 2>& position : row){
            std::cout << "{";
            std::cout << position[0] << ", " << position[1];
            std::cout << "} ";
        }
        std::cout << "}\n";
    }
    std::cout << "}";
}

std::array<double, 2> Simulation::solve2x2Matrix(double a, double b, double c, double d, double e, double f) const{
    double det = (a * d) - (c * b);
    double det_x = (e * d) - (f * b);
    double det_y = (a * f) - (c * e);

    double x = det_x / det;
    double y = det_y / det;

    return {x, y};
}