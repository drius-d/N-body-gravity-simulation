#include "particle.hpp"

Particle::Particle(const double m, const std::array<double, 2>& p, const std::array<double, 2>& v)
: mass(m), position(p), velocity(v) {}

void Particle::updateParticle(const std::array<double, 2>& force, const double dt) {
    updateAcceleration(force);
    updateVelocity(force, dt);
    updatePosition(force, dt);
}

void Particle::updateAcceleration(const std::array<double, 2>& force){
    for (int i = 0; i < force.size(); i++){
        acceleration[i] = force[i] / mass;
    }
}

void Particle::updateVelocity(const std::array<double, 2>& acceleration, const double dt){
    for (int i = 0; i < acceleration.size(); i++){
        velocity[i] += acceleration[i] * dt;
    }
}

void Particle::updatePosition(const std::array<double, 2>& velocity, const double dt){
    for (int i = 0; i < velocity.size(); i++){
        position[i] += velocity[i] * dt;
    }    
}

std::array<double, 2> Particle::getPosition() const{
    return position;
}

double Particle::getMass() const{
    return mass;
}