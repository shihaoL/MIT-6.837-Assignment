#pragma once
#pragma once
#pragma once
#include "material.h"
#include "matrix.h"
#include "perlin_noise.h"
class Wood :
	public Material
{
private:
	int oct;
	float freq, amp;
	Matrix m;
	Material* mat1 = nullptr;
	Material* mat2 = nullptr;
	bool one_or_two(double noise)const {
		double t = noise - int(noise);
		return t < 0.5;
	}
public:
	Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) :
		m(*m), mat1(mat1), mat2(mat2), oct(octaves), freq(frequency), amp(amplitude), Material(*mat1) {}
	~Wood() {}
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
		for (int i = 1; i <= oct; i++) {
			noise += PerlinNoise::noise(i*x, i*y, i*z) / i;
		}
		noise = sin(freq*x + amp*noise);
		if (one_or_two(noise)) {
			return mat1->Shade(ray, hit, dirToLight, lightColor);
		}
		else return mat2->Shade(ray, hit, dirToLight, lightColor);
	}
};

