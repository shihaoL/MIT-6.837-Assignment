#pragma once
#include "object3d.h"
#include <vector>
class Group :public Object3D {
public:
	void addObject(int index, Object3D *obj) {
		assert(index < objs.size());
		objs[index] = obj;
		if (obj->getBoundingBox()) {
			if (bb == nullptr) bb = new BoundingBox(*obj->getBoundingBox());
			else {
				bb->Extend(obj->getBoundingBox());
			}
		}
	}
	void insertIntoGrid(Grid *g, Matrix *m) {
		for (auto o : objs) {
			o->insertIntoGrid(g, m);
		}
	}
	Group(int size) {
		setBoundingBox();
		objs.resize(size);
	}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		bool res = false;
		for (auto o : objs) {
			bool ok = o->intersect(r, h, tmin);
			res = res || ok;
		}
		return res;
	}
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) override {
		for (auto o : objs) {
			if (o->intersect(r, h, tmin)) return true;
		}
		return false;
	}
	virtual void paint() {
		for (auto o : objs) {
			o->paint();
		}
	}
private:
	std::vector<Object3D*> objs;
	void setBoundingBox()override {
		bb = nullptr;
	}
};