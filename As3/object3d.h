#pragma once 
#include "ray.h"
#include "hit.h"
#include "material.h"
#include <GL/GL.h>
#include <GL/GLU.h>
#define M_PI 3.14159
class Object3D {
public:
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
	Material* material = nullptr;
	Object3D() = default;
	Object3D(Material* _material):material(_material){}
	virtual void paint() = 0;
};