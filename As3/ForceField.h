#pragma once
#include "vectors.h"
class ForceField {
public:
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;
};

class GravityForceField:public ForceField {
private:
	Vec3f gravity;
public:
	GravityForceField(Vec3f gravity) :gravity(gravity) {}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const{
		return gravity;
	}
};

class ConstantForceField :public ForceField {
private:
	Vec3f force;
public:
	ConstantForceField(Vec3f force):force(force){}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const {
		return 1.0f / mass * force;
	}
};

class RadialForceField :public ForceField {
private:
	float magnitude;
public:
	RadialForceField(float magnitude):magnitude(magnitude){}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const {
		auto dir = position;
		dir.Normalize();
		return -position.Length()*magnitude / mass * dir;
	}
};

class VerticalForceField :public ForceField {
private:
	float magnitude;
public:
	VerticalForceField(float magnitude) :magnitude(magnitude) {}
	virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const {
		auto dir = Vec3f(0, -1, 0);
		return magnitude * position.y() / mass * dir;
	}
};