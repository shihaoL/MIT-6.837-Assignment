#pragma once
#include "scene_parser.h"
#include "ray.h"
#include "hit.h"
#include "Grid.h"
#include "matrix.h"
#include "group.h"
class RayTracer
{
public:
	RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows,
		bool shade_back,bool visual_grid,bool use_grid, int nx,int ny,int nz
	):
		s(s),max_bounces(max_bounces),cutoff_weight(cutoff_weight),shadows(shadows),shade_back(shade_back),
		visual_grid(visual_grid),nx(nx),ny(ny),nz(nz)
	{
		if (!use_grid) {
			grid = nullptr;
			return;
		}
		auto g = s->getGroup();
		grid = new Grid(g->getBoundingBox(), nx, ny, nz);
		Matrix m;
		m.SetToIdentity();
		g->insertIntoGrid(grid, &m);
	}
	~RayTracer() {}
	Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight,
		float indexOfRefraction, Hit &hit) const;
	Grid* grid=nullptr;
private:
	SceneParser * s;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
	bool shade_back;
	bool visual_grid = false;
	int nx, ny, nz;

	Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const ;
	bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
		float index_i, float index_t, Vec3f &transmitted) const;
};

