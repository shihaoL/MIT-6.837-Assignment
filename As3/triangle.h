#pragma once
#include "object3d.h"
#include <algorithm>
class Triangle :public Object3D {
public:
	Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m) :Object3D(m), _a(a), _b(b), _c(c) {
		Vec3f::Cross3(_normal,b-a,c-a);
		_normal.Normalize();
		setBoundingBox();
	}
	void insertIntoGrid(Grid *g, Matrix *m) override {
		if (m != nullptr) {
			g->transform_into_Grid(bb, m, this);
			//g->intoGrid(bb->getMin(), bb->getMax(), this);
		}
	}
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
		return intersect(r, h, tmin);
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
		if (t > tmin&&beta+gama<=1&&beta>=0&&gama>=0){
			if (t < h.getT()) {
				h.set(t, material, _normal, r);
				return true;
			}
		}
		return false;
	}
	virtual void paint() {
		if (material) material->glSetMaterial();
		glBegin(GL_TRIANGLES);
		glNormal3f(_normal.x(), _normal.y(), _normal.z());
		glVertex3f(_a.x(),_a.y(),_a.z());
		glVertex3f(_b.x(), _b.y(), _b.z());
		glVertex3f(_c.x(),_c.y(),_c.z());
		glEnd();
	}
private:
	Vec3f _a, _b, _c, _normal;
	float determinant3(const Vec3f& a, const Vec3f& b, const Vec3f& c) {
		return a.x()*b.y()*c.z() + a.y()*b.z()*c.x() + a.z()*b.x()*c.y() -
			a.x()*c.y()*b.z() - a.y()*b.x()*c.z() - a.z()*b.y()*c.x();
	}
	void setBoundingBox()override {
		using std::max;
		using std::min;
		float bmax[3], bmin[3];
		for (int i = 0; i < 3; i++) {
			if (_a[i] >= _b[i] && _a[i] >= _c[i]) bmax[i] = _a[i];
			if (_b[i] >= _a[i] && _b[i] >= _c[i]) bmax[i] = _b[i];
			if (_c[i] >= _a[i] && _c[i] >= _b[i]) bmax[i] = _c[i];
			if (_a[i] <= _b[i] && _a[i] <= _c[i]) bmin[i] = _a[i];
			if (_b[i] <= _a[i] && _b[i] <= _c[i]) bmin[i] = _b[i];
			if (_c[i] <= _a[i] && _c[i] <= _b[i]) bmin[i] = _c[i];
		}
		Vec3f _max(bmax[0], bmax[1], bmax[2]);
		Vec3f _min(bmin[0], bmin[1], bmin[2]);
		bb = new BoundingBox(_min, _max);
	}
};