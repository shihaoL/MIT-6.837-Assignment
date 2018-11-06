#pragma once
#include "object3d.h"
#include <vector>
class Group :public Object3D {
public:
	void addObject(int index, Object3D *obj) {
		assert(index < objs.size());
		objs[index] = obj;
	}
	Group(int size) {
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
	virtual void paint() {
		for (auto o : objs) {
			o->paint();
		}
	}
private:
	std::vector<Object3D*> objs;
};