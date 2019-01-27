#include "RayTracer.h"
#include "group.h"
#include"light.h"
#include "rayTree.h"
static constexpr float epsilon = 0.001f;
Vec3f RayTracer::traceRay(Ray & ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit & hit) const
{
	if (weight <= cutoff_weight) return{ 0,0,0 };
	if (bounces > max_bounces) return { 0,0,0 };
	Object3D* group = s->getGroup();
	if (visual_grid) group = grid;
	bool hited = group->intersect(ray, hit, tmin);
	if (hited) {
		bool back = hit.getNormal().Dot3(ray.getDirection()) > 0;
		if (back && !shade_back) return { 0,0,0 };
		if (back) {
			auto normal = hit.getNormal();
			normal.Negate();
			hit.set(hit.getT(), hit.getMaterial(), normal, ray);
		}
		Vec3f res(0, 0, 0);
		auto hit_point = hit.getIntersectionPoint();
		auto material = hit.getMaterial();
		//ambient
		res += s->getAmbientLight()*material->getDiffuseColor();
		int light_num = s->getNumLights();
		for (int i = 0; i < light_num; i++) {
			auto light = s->getLight(i);
			Vec3f light_dir, light_col;
			float distance_to_light;
			light->getIllumination(hit_point, light_dir, light_col, distance_to_light);
			//shadow cast
			Ray shadow_ray(hit_point, light_dir);
			Hit shadow_hit(distance_to_light, NULL, { 0,1,0 });
			if (visual_grid||!group->intersectShadowRay(shadow_ray, shadow_hit, epsilon)) {
				res += material->Shade(ray, hit, light_dir, light_col);
			}
			else {
				RayTree::AddShadowSegment(shadow_ray, epsilon, shadow_hit.getT());
			}
		}
		//reflection 
		if (material->getReflectionColor().Length() > epsilon) {
			auto reflectDir = mirrorDirection(hit.getNormal(), ray.getDirection());
			auto cur_weight = weight * material->getReflectionColor().Length();
			auto cur_depth = bounces + 1;
			Ray reflect_ray(hit_point, reflectDir);
			Hit reflect_hit(INFINITY, NULL, { 0,1,0 });
			auto reflectColor = traceRay(reflect_ray, epsilon, cur_depth, cur_weight, indexOfRefraction, reflect_hit);
			RayTree::AddReflectedSegment(reflect_ray, epsilon, reflect_hit.getT());
			res += material->getReflectionColor()*reflectColor;
		}
		//refraction
		if (material->getTransparentColor().Length() > epsilon) {
			auto normal = hit.getNormal();
			normal.Normalize();
			auto income = ray.getDirection();
			income.Normalize();
			bool is_into_obj = normal.Dot3(income) < 0;
			if (back) is_into_obj = !is_into_obj;
			income.Negate();
			float index_i = indexOfRefraction;
			float index_t = is_into_obj ? material->getIndexOfRfefraction() : 1.0f;
			Vec3f refractDir;
			if (transmittedDirection(normal, income, index_i, index_t, refractDir)) {
				Ray refract_ray(hit_point, refractDir);
				Hit refract_hit(INFINITY, NULL, { 0,1,0 });
				auto cur_weight = weight * material->getTransparentColor().Length();
				auto cur_depth = bounces + 1;
				auto refractColor = traceRay(refract_ray, epsilon, cur_depth, cur_weight, index_t, refract_hit);
				RayTree::AddTransmittedSegment(refract_ray, epsilon, refract_hit.getT());
				res += material->getTransparentColor()*refractColor;
			}

		}
		return res;
	}
	else {
		return s->getBackgroundColor();
	}
}

Vec3f RayTracer::mirrorDirection(const Vec3f & normal, const Vec3f & incoming) const
{
	auto I = incoming;
	I.Normalize();
	auto N = normal;
	N.Normalize();
	auto R = I - 2 * (I.Dot3(N))*N;
	R.Normalize();
	return R;
}

bool RayTracer::transmittedDirection(const Vec3f & normal, const Vec3f & incoming, float index_i, float index_t, Vec3f & transmitted) const
{
	float nr = index_i / index_t;
	auto n_dot_i = normal.Dot3(incoming);
	auto b = 1 - nr * nr*(1 - n_dot_i * n_dot_i);
	if (b < 0) return false;
	auto T = (nr*n_dot_i - sqrt(b))*normal - nr * incoming;
	T.Normalize();
	transmitted = T;
	return true;
}
