#pragma once
#include "object3d.h"
#include "matrix.h"
class Transform :public Object3D {
private:
	Object3D * obj;
	Matrix _m;
	Matrix _m_inv;
	Matrix _m_inv_tr;
public:
	Transform(Matrix &m, Object3D *o) :_m(m), obj(o) {
		_m_inv = _m;
		_m_inv.Inverse();
		_m_inv_tr = _m_inv;
		_m_inv_tr.Transpose();
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
};