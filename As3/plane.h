#pragma once
#include "object3d.h"
#include "raytracing_stats.h"
#include "transform.h"
class Plane :public Object3D {
private:
	const int big_num = 10000;
	Vec3f _normal;
	float _d;
	void setBoundingBox()override{
		bb = nullptr;
	}
public:
	Plane(Vec3f &normal, float d, Material *m):_normal(normal),_d(d), Object3D(m){
		setBoundingBox();
	}
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
		return intersect(r, h, tmin);
	}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		RayTracingStats::IncrementNumIntersections();
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
	virtual void paint() {
		if (material) material->glSetMaterial();
		Vec3f tmp{ 1,0,0 };
		if (abs(tmp.Dot3(_normal)) == 1) tmp = Vec3f(0, 1, 0);
		Vec3f b1, b2;
		Vec3f::Cross3(b1, _normal, tmp);
		Vec3f::Cross3(b2, _normal, b1);
		b1.Normalize(); b2.Normalize();
		Vec3f plane_center = _d * _normal;
		Vec3f t1=big_num*b1, t2=big_num*b2;
		Vec3f p1 = plane_center+ t1 + t2, p2 = plane_center + t1 - t2, p3 = plane_center + -1*t1 - t2, p4 = plane_center + -1*t1 + t2;
		glBegin(GL_QUADS);
		glNormal3f(_normal.x(), _normal.y(), _normal.z());
		glVertex3f(p1.x(), p1.y(), p1.z());
		glVertex3f(p2.x(), p2.y(), p2.z());
		glVertex3f(p3.x(), p3.y(), p3.z());
		glVertex3f(p4.x(), p4.y(), p4.z());
		glEnd();
	}
	void insertIntoGrid(Grid *g, Matrix *m)override {
		Transform* t = new Transform(*m, this);
		g->intoInfinityGrid(t);
	}
};