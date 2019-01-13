#pragma once
#include "scene_parser.h"
#include "ray.h"
#include "hit.h"
class RayTracer
{
public:
	RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows,
		bool shade_back
	):
		s(s),max_bounces(max_bounces),cutoff_weight(cutoff_weight),shadows(shadows),shade_back(shade_back)
	{}
	~RayTracer() {}
	Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
		float indexOfRefraction, Hit &hit) const;
private:
	SceneParser * s;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
	bool shade_back;

	Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const ;
	bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
		float index_i, float index_t, Vec3f &transmitted) const;
};

