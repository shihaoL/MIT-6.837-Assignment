#pragma once
#include "object3d.h"
class Triangle :public Object3D {
public:
	Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) :Object3D(m), _a(a), _b(b), _c(c) {
		Vec3f::Cross3(_normal,b-a,c-a);
		_normal.Normalize();
	}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		Vec3f dir = r.getDirection();
		Vec3f origin = r.getOrigin();
		if (dir.Dot3(_normal) == 0) return false;
		Vec3f ta = _a - _b;
		Vec3f tb = _a - _c;
		Vec3f tc = dir;
		Vec3f td = _a - origin;
		float D = determinant3(ta, tb, tc);
		float beta = determinant3(td,tb,tc) / D;
		float gama = determinant3(ta, td, tc) / D;
		float t = determinant3(ta, tb, td) / D;
		if (t > tmin&&beta+gama<1&&beta>0&&gama>0){
			if (t < h.getT()) {
				h.set(t, material, _normal, r);
				return true;
			}
		}
		return false;
	}
private:
	Vec3f _a, _b, _c, _normal;
	float determinant3(const Vec3f& a, const Vec3f& b, const Vec3f& c) {
		return a.x()*b.y()*c.z() + a.y()*b.z()*c.x() + a.z()*b.x()*c.y() -
			a.x()*c.y()*b.z() - a.y()*b.x()*c.z() - a.z()*b.y()*c.x();
	}
};