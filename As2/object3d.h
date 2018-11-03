#pragma once 
#include "ray.h"
#include "hit.h"
#include "material.h"
class Object3D {
public:
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
	Material* material = nullptr;
	Object3D() = default;
	Object3D(Material* _material):material(_material){}
};