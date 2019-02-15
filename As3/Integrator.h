#pragma once
#include "particle.h"
#include "ForceField.h"
class Integrator {
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt) = 0;
	virtual Vec3f getColor() = 0;
};

class EulerIntegrator :public Integrator {
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt) {
		auto pn = particle->getPosition();
		auto vn = particle->getVelocity();
		auto pn_1 = pn + vn * dt;
		auto a = forcefield->getAcceleration(pn, particle->getMass(), t);
		auto vn_1 = vn + a*dt;
		particle->setPosition(pn_1);
		particle->setVelocity(vn_1);
		particle->increaseAge(dt);
	}
	virtual Vec3f getColor() {
		return { 0,1,0 };
	}
};

class MidpointIntegrator :public Integrator {
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt) {
		auto pn = particle->getPosition();
		auto vn = particle->getVelocity();
		auto pm = pn + dt / 2*vn;
		auto a = forcefield->getAcceleration(pn, particle->getMass(), t);
		auto vm = vn + dt / 2 * a;
		auto pn1 = pn + vm * dt;
		auto am = forcefield->getAcceleration(pm, particle->getMass(), t + dt / 2);
		auto vn1 = vn + am * dt;
		particle->setPosition(pn1);
		particle->setVelocity(vn1);
		particle->increaseAge(dt);
	}
	virtual Vec3f getColor() {
		return { 1,0,0 };
	}
};
