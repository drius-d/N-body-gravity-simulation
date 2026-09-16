void Simulation::processCollisions(){
    double remainingTime = dt;

    // loop through all collisions 

    while (remainingTime > 0){

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

                // r is the distance between the centre of the particles
                std::array<double, 2> r;
                r[0] = otherParticlePosition[0] - currentParticlePosition[0];
                r[1] = otherParticlePosition[1] - currentParticlePosition[1];
                
                // v is the relative velocity of the two particles
                std::array<double, 2> v;
                v[0] = otherParticleVelocity[0] - currentParticleVelocity[0];
                v[1] = otherParticleVelocity[1] - currentParticleVelocity[1];

                double distance = std::sqrt(r[0]*r[0] + r[1]*r[1]);

                // checks if particles already overlapping 
                if (distance < R){

                    if (distance == 0){
                        continue;
                    }

                    // calculating u2 and u1, the components of velocity in line of the contact normal
                    std::array<double, 2> normal;
                    normal[0] = r[0] / distance;
                    normal[1] = r[1] / distance;

                    double u1 = std::inner_product(currentParticleVelocity.begin(), currentParticleVelocity.end(), unitVector.begin(), 0.0);
                    double u2 = std::inner_product(otherParticleVelocity.begin(), otherParticleVelocity.end(), unitVector.begin(), 0.0);

                    // Resolving overlap by moving the particles apart by half penetration
                    double penetration = R - distance;

                    currentParticlePosition[0] -= normal[0] * penetration / 2;
                    currentParticlePosition[1] -= normal[1] * penetration / 2;

                    otherParticlePosition[0] += normal[0] * penetration / 2;
                    otherParticlePosition[1] += normal[0] * penetration / 2;

                    overlapResolved = true;

                    break;
                }

                // some complicated maths: basically using some geometry you can determine that
                // the equation for the time of collision for any two particles is given by a quadratic
                // at^2 + bt + c = 0, where a is dot product of v and v, b is 2 * dot product of r and v,
                // c is dot product of r and r - R^2

                double a = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
                double b = 2 * std::inner_product(r.begin(), r.end(), v.begin(), 0.0);
                double c = std::inner_product(r.begin(), r.end(), r.begin(), 0.0) - R*R;
                
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
            
            // calculates unit vector pointing between particle centres

            std::array<double, 2> unitVector;
            unitVector[0] = relPosition[0] / distance;
            unitVector[1] = relPosition[1] / distance;

            // gets particle masses and velocities

            double m1 = pParticle->getMass();
            double m2 = pOther->getMass();
            std::array<double, 2>& currentParticleVelocity = pParticle->getVelocity();
            std::array<double, 2>& otherParticleVelocity = pOther->getVelocity();

            // calculates component of velocity for both particles along unit vector using inner product

            double u1 = std::inner_product(currentParticleVelocity.begin(), currentParticleVelocity.end(), unitVector.begin(), 0.0);
            double u2 = std::inner_product(otherParticleVelocity.begin(), otherParticleVelocity.end(), unitVector.begin(), 0.0);
            
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

            currentParticleVelocity[0] += (v1 - u1) * unitVector[0];
            currentParticleVelocity[1] += (v1 - u1) * unitVector[1];
            
            otherParticleVelocity[0] += (v2 - u2) * unitVector[0];
            otherParticleVelocity[1] += (v2 - u2) * unitVector[1];

        }
        remainingTime -= earliestCollisionTime;
    }
}        
        
        