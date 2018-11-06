#pragma once
#include "ray.h"
#include "vectors.h"

class Camera {
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
	virtual void glInit(int w, int h) = 0;
	virtual void glPlaceCamera(void) = 0;
	virtual void dollyCamera(float dist) = 0;
	virtual void truckCamera(float dx, float dy) = 0;
	virtual void rotateCamera(float rx, float ry) = 0;
};

class OrthographicCamera :public Camera {
private:
	Vec3f up, horizontal, dir;
	Vec3f world_up;
	Vec3f pos;
	float size;
public:
	OrthographicCamera(Vec3f _pos, Vec3f _dir, Vec3f _up, float _size):pos(_pos),size(_size),world_up(_up){
		dir = _dir;
		Vec3f::Cross3(horizontal, _dir, _up);
		Vec3f::Cross3(up, horizontal, _dir);
		dir.Normalize();
		horizontal.Normalize();
		up.Normalize();
		world_up.Normalize();
	}
	virtual Ray generateRay(Vec2f point) {
		Vec3f leftbottom = pos - (size*up*0.5) - (size*horizontal*0.5);
		Vec3f origin = leftbottom + point.x()*horizontal*size + point.y()*up*size;
		return Ray(origin, dir);
	}
	virtual float getTMin() const {
		return -INFINITY;
	}
	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);
};

class PerspectiveCamera:public Camera {
public:
	PerspectiveCamera(const Vec3f& _center, const Vec3f &_direction, const Vec3f &_up, float _angle):center(_center),angle(_angle),world_up(_up) {
		direction = _direction;
		Vec3f::Cross3(horizontal, _direction, _up);
		Vec3f::Cross3(up, horizontal, _direction);
		direction.Normalize();
		horizontal.Normalize();
		up.Normalize();
		world_up.Normalize();
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
	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);
private:
	Vec3f center, direction, up, horizontal;
	Vec3f world_up;
	float angle;
};