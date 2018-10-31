#pragma once
#include "object3d.h"
class Sphere :public Object3D {
private:
	float radius;
	Vec3f center;
public:
	Sphere(Vec3f _center, float _radius, Material* _material) :Object3D(_material),radius(_radius), center(_center){}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		Vec3f origin = r.getOrigin() - center;
		Vec3f dir = r.getDirection();
		float a = 1, b = dir.Dot3(origin) * 2, c = origin.Dot3(origin) - radius * radius;
		float d = b * b - 4 * a*c;
		float t = 0;
		if (d < 0) return false;
		if (d == 0) {
			t = -b / (2 * a);
		}
		else {
			d = sqrt(d);
			t = (-b - d) / (2 * a);
		}
		if (t > tmin) {
			if (h.getT() > t) {
				h.set(t, material, r);
			}
			return true;
		}
		return false;
	}
};