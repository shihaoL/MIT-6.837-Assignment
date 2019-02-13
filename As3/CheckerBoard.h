#pragma once
#include "material.h"
#include "matrix.h"
class CheckerBoard :
	public Material
{
private:
	inline bool odd(int x) const{
		return abs(x) % 2 == 1;
	}
	Matrix m;
	Material* mat1=nullptr;
	Material* mat2=nullptr;
	bool one_or_two(int x, int y, int z)const {
		return odd(x) ^ odd(y) ^ odd(z);
	}
public:
	CheckerBoard(Matrix *m, Material *mat1, Material *mat2):
		m(*m),mat1(mat1),mat2(mat2),Material(*mat1){}
	~CheckerBoard();
	virtual void glSetMaterial(void)const{
		mat1->glSetMaterial();
	}
	virtual Vec3f Shade
	(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const {
		auto pos = hit.getIntersectionPoint();
		m.Transform(pos);
		int x = floor(pos.x());
		int y = floor(pos.y());
		int z = floor(pos.z());
		if (one_or_two(x, y, z)) {
			return mat1->Shade(ray, hit, dirToLight, lightColor);
		}
		else return mat2->Shade(ray, hit, dirToLight, lightColor);
	}
};

