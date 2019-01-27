#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"
#include "glCanvas.h"
// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

  // CONSTRUCTORS & DESTRUCTOR
  Material(const Vec3f &d_color) { diffuseColor = d_color; }
  Material(const Vec3f &d_color, const Vec3f &reflectiveColor, const Vec3f& transparentColor,float indexOfRefraction)
	  :diffuseColor(d_color), reflectiveColor(reflectiveColor), transparentColor(transparentColor),indexOfRefraction(indexOfRefraction) {}
  virtual ~Material() {}

  // ACCESSORS
  virtual Vec3f getDiffuseColor() const { return diffuseColor; }
  virtual Vec3f getReflectionColor()const {
	  return reflectiveColor;
  }
  virtual Vec3f getTransparentColor() const {
	  return transparentColor;
  }
  virtual float getIndexOfRfefraction() const {
	  return indexOfRefraction;
  }
  virtual Vec3f Shade
  (const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	  const Vec3f &lightColor) const = 0;
  virtual void glSetMaterial(void) const = 0;
protected:

  // REPRESENTATION
  Vec3f diffuseColor;
  Vec3f reflectiveColor, transparentColor;
  float indexOfRefraction;
};

class PhongMaterial :public Material {
public:
	PhongMaterial(const Vec3f &_diffuseColor,
		const Vec3f &_specularColor,
		float _exponent,
		const Vec3f &reflectiveColor,
		const Vec3f &transparentColor,
		float indexOfRefraction):
		Material(_diffuseColor,reflectiveColor,transparentColor,indexOfRefraction),specularColor(_specularColor),exponent(_exponent)
	{

	}
	Vec3f getSpecularColor()const {
		return specularColor;
	}
	virtual Vec3f Shade
	(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const {
		Vec3f view_dir = ray.getDirection();
		view_dir.Normalize();
		view_dir.Negate();
		Vec3f h = view_dir + dirToLight;
		h.Normalize();
		Vec3f specular, diffuse;
		float dot = dirToLight.Dot3(hit.getNormal());
		diffuse = diffuseColor * lightColor*dot;
		if (dot < 0) diffuse = { 0,0,0 };
		specular = pow(h.Dot3(hit.getNormal()), exponent)*specularColor*lightColor;
#if SPECULAR_FIX
		specular *= dot;
		if (dot < 0 && specular.x()>0) specular = { 0,0,0 };
#else
		if (dot < 0) specular = { 0,0,0 };
#endif
		return diffuse + specular;
	}
	virtual void glSetMaterial(void)const;
private:
	Vec3f specularColor;
	float exponent;
};

// ====================================================================
// ====================================================================

extern PhongMaterial RED;
extern PhongMaterial WHITE;
extern PhongMaterial BLUE;
extern PhongMaterial GREED;
extern PhongMaterial GRAY;
#endif
