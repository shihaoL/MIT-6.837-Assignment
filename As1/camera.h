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