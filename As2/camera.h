#pragma once
#include "ray.h"
#include "vectors.h"
class Camera {
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
};

class OrthographicCamera :public Camera {
private:
	Vec3f up, horizontal, dir;
	Vec3f pos;
	float size;
public:
	OrthographicCamera(Vec3f _pos, Vec3f _dir, Vec3f _up, float _size):pos(_pos),size(_size){
		dir = _dir;
		Vec3f::Cross3(horizontal, _dir, _up);
		Vec3f::Cross3(up, horizontal, _dir);
		dir.Normalize();
		horizontal.Normalize();
		up.Normalize();
	}
	virtual Ray generateRay(Vec2f point) {
		Vec3f leftbottom = pos - (size*up*0.5) - (size*horizontal*0.5);
		Vec3f origin = leftbottom + point.x()*horizontal*size + point.y()*up*size;
		return Ray(origin, dir);
	}
	virtual float getTMin() const {
		return -INFINITY;
	}
};

class PerspectiveCamera:public Camera {
public:
	PerspectiveCamera(const Vec3f& _center, const Vec3f &_direction, const Vec3f &_up, float _angle):center(_center),angle(_angle) {
		direction = _direction;
		Vec3f::Cross3(horizontal, _direction, _up);
		Vec3f::Cross3(up, horizontal, _direction);
		direction.Normalize();
		horizontal.Normalize();
		up.Normalize();
	}
	virtual Ray generateRay(Vec2f point) {
		float x = point.x()-0.5;
		float y = point.y()-0.5;
		float z = 0.5 / tan(angle / 2);
		Vec3f p = horizontal * x + up * y + direction * z;
		Vec3f dir = p;
		dir.Normalize();
		return Ray(center, dir);
	}
	virtual float getTMin() const {
		return 0;
	}
private:
	Vec3f center, direction, up, horizontal;
	float angle;
};