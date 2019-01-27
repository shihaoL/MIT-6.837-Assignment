#pragma once 
#include "ray.h"
#include "hit.h"
#include "material.h"
#include <GL/GL.h>
#include <GL/GLU.h>
#include"boundingbox.h"
#define M_PI 3.141592f
class Grid;
class Object3D {
public:
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) = 0;
	Material* material = nullptr;
	Object3D() = default;
	Object3D(Material* _material):material(_material){}
	virtual void paint() = 0;
	BoundingBox* getBoundingBox() {
		return bb;
	}
	virtual ~Object3D() {
		delete bb;
	}
	virtual void insertIntoGrid(Grid *g, Matrix *m) {

	}
protected:
	virtual void setBoundingBox() = 0;
	BoundingBox* bb = nullptr;
};