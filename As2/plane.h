#pragma once
#include "object3d.h"
class Plane :public Object3D {
private:
	Vec3f _normal;
	float _d;
public:
	Plane(Vec3f &normal, float d, Material *m):_normal(normal),_d(d), Object3D(m){}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		Vec3f dir = r.getDirection();
		Vec3f origin = r.getOrigin();
		if (dir.Dot3(_normal) == 0) return false;
		else {
			float t = (_d - origin.Dot3(_normal)) / (dir.Dot3(_normal));
			if (t > tmin) {
				if (t < h.getT()) {
					h.set(t, material, _normal, r);
					return true;
				}
			}
		}
		return false;
	}
};