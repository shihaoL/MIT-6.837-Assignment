#pragma once
#include "particle.h"
#include "random.h"
#include <time.h>
class Generator {
protected:
	Vec3f color, dead_color;
	float lifespan;
	int desired_num_particles;
	float mass;
	Random* r;
public:
	Generator() {
		r = new Random(time(NULL));
	}
	// initialization
	virtual void SetColors(Vec3f color, Vec3f dead_color, float color_randomness) {
		this->color = color + color_randomness * r->randomVector();
		this->dead_color = dead_color + color_randomness * r->randomVector();
	}
	virtual void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles){
		this->lifespan = lifespan + lifespan_randomness * r->next();
		this->desired_num_particles = desired_num_particles;
	}
	virtual void SetMass(float mass, float mass_randomness){
		this->mass = mass + mass_randomness * r->next();
	}

	// on each timestep, create some particles
	virtual int numNewParticles(float current_time, float dt) const{
		return dt*desired_num_particles / lifespan;
	}
	virtual Particle* Generate(float current_time, int i) = 0;
	// for the gui
	virtual void Paint() const{}
	virtual void Restart(){
		delete r;
		r = new Random(time(NULL));
	}
};

class HoseGenerator :public Generator {
private:
	Vec3f position,velocity;
	float position_randomness, velocity_randomness;
public:
	HoseGenerator(Vec3f position, float position_randomness, Vec3f velocity, float velocity_randomness) :
		position(position), position_randomness(position_randomness), velocity(velocity), velocity_randomness(velocity_randomness) {}
	virtual Particle* Generate(float current_time, int i) {
		return new Particle(
			position + position_randomness * r->randomVector(), velocity + velocity_randomness * r->randomVector(), color, dead_color, mass, lifespan
		);
	}
};

class RingGenerator :public Generator {
private:
	float position_randomness, velocity_randomness;
	Vec3f velocity;
public:
	RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness):position_randomness(position_randomness),velocity(velocity),velocity_randomness(velocity_randomness){}
	virtual int numNewParticles(float current_time, float dt) const {
		return dt * desired_num_particles*current_time/ lifespan;
	}
	virtual Particle* Generate(float current_time, int i) {
		auto p = position_randomness*r->randomVector();
		p[1] = 0;
		p.Normalize();
		p = current_time* p;
		return new Particle(
			p, velocity + velocity_randomness * r->randomVector(), color, dead_color, mass, lifespan
		);
	}
};

