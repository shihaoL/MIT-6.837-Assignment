#pragma once
#include "object3d.h"
#include "matrix.h"
class Transform :public Object3D {
private:
	Object3D * obj;
	Matrix _m;
	Matrix _m_inv;
	Matrix _m_inv_tr;
	void setBoundingBox()override {
		auto t_bb = obj->getBoundingBox();
		if (t_bb == nullptr) return;
		auto t_max = t_bb->getMax(), t_min = t_bb->getMin();
		Vec3f p[8] = {
			{ t_max[0],t_max[1],t_max[2] },
		{ t_max[0],t_max[1],t_min[2] },
		{ t_max[0],t_min[1],t_max[2] },
		{ t_max[0],t_min[1],t_min[2] },
		{ t_min[0],t_max[1],t_max[2] },
		{ t_min[0],t_max[1],t_min[2] },
		{ t_min[0],t_min[1],t_max[2] },
		{ t_min[0],t_min[1],t_min[2] }
		};
		float maxx = -INFINITY, maxy = -INFINITY, maxz = -INFINITY, minx = INFINITY, miny = INFINITY, minz = INFINITY;
		for (int i = 0; i < 8; i++) {
			_m.Transform(p[i]);
			if (p[i].x() > maxx) maxx = p[i].x();
			if (p[i].x() < minx) minx = p[i].x();
			if (p[i].y() > maxy) maxy = p[i].y();
			if (p[i].y() < miny)miny = p[i].y();
			if (p[i].z() > maxz) maxz = p[i].z();
			if (p[i].z() < minz) minz = p[i].z();
		}
		bb = new BoundingBox({ minx,miny,minz }, { maxx,maxy,maxz });
	}
public:
	Transform(Matrix &m, Object3D *o) :_m(m), obj(o) {
		_m_inv = _m;
		_m_inv.Inverse();
		_m_inv_tr = _m_inv;
		_m_inv_tr.Transpose();
		setBoundingBox();
	}
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
		return intersect(r, h, tmin);
	}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		Vec3f orgin = r.getOrigin();
		Vec3f dir = r.getDirection();
		_m_inv.Transform(orgin);
		_m_inv.TransformDirection(dir);
		/*float scale = dir.Length();
		dir.Normalize();*/
		Ray nr(orgin, dir);
		bool hit = obj->intersect(nr, h, tmin);
 		if (hit) {
			Vec3f world_normal = h.getNormal();
			_m_inv_tr.TransformDirection(world_normal);
			world_normal.Normalize();
			/*float nt = h.getT() / scale;*/
			h.set(h.getT(), h.getMaterial(), world_normal, r);
		}
		return hit;
	}
	virtual void paint() {
		glPushMatrix();
		GLfloat *glMatrix = _m.glGet();
		glMultMatrixf(glMatrix);
		delete[] glMatrix;
		obj->paint();
		glPopMatrix();
	}
	void insertIntoGrid(Grid *g, Matrix *m) override {
		Matrix tmpm = (*m)*_m;
		obj->insertIntoGrid(g, &tmpm);
	}
};