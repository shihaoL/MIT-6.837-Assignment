#pragma once
#include "material.h"
#include "matrix.h"
#include "perlin_noise.h"
class Noise :
	public Material
{
private:
	int oct;
	Matrix m;
	Material* mat1 = nullptr;
	Material* mat2 = nullptr;
	bool one_or_two(double noise)const {
		//double t = noise - int(noise);
		return noise < 0.1;
	}
public:
	Noise(Matrix *m, Material *mat1, Material *mat2, int octaves):
		m(*m), mat1(mat1), mat2(mat2), oct(octaves),Material(*mat1) {}
	~Noise() {}
	virtual void glSetMaterial(void)const {
		mat1->glSetMaterial();
	}
	virtual Vec3f Shade
	(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const {
		auto pos = hit.getIntersectionPoint();
		m.Transform(pos);
		double x = pos.x();
		double y = pos.y();
		double z = pos.z();
		double noise = 0;
		for (int i = 0; i < oct; i++) {
			int fact = 1 << i;
			noise += PerlinNoise::noise(fact*x, fact*y, fact*z) / fact;
		}
		noise += 0.5;
		noise = noise < 0 ? 0 : (noise > 1 ? 1 : noise);
		return (noise)*mat1->Shade(ray, hit, dirToLight, lightColor) + (1-noise)*mat2->Shade(ray, hit, dirToLight, lightColor);
		/*if (one_or_two(noise)) {
			return mat1->Shade(ray, hit, dirToLight, lightColor);
		}
		else return mat2->Shade(ray, hit, dirToLight, lightColor);*/
	}
};

