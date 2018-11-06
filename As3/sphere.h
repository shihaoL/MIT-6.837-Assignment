#pragma once
#include "object3d.h"

extern float theta_step, phi_step;
extern bool gouraud;
class Sphere :public Object3D {
private:
	float radius;
	Vec3f center;
	Vec3f pt2localxyz(float phi, float theta) {
		float rphi = phi /180 * M_PI;
		float rtheta = theta / 180 * M_PI;
		float y = sin(rphi)*radius;
		float z = cos(rtheta)*cos(rphi)*radius;
		float x = sin(rtheta)*cos(rphi)*radius;
		return { x,y,z };
	}
	Vec3f pt2xyz(float phi, float theta) {
		Vec3f pos = pt2localxyz(phi, theta);
		return pos+center;
	}
	Vec3f pt2Normal(float phi, float theta) {
		Vec3f n = pt2localxyz(phi, theta);
		n.Normalize();
		return n;
	}
public:
	Sphere(Vec3f _center, float _radius, Material* _material) :Object3D(_material),radius(_radius), center(_center){}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		Vec3f origin = r.getOrigin() - center;
		Vec3f dir = r.getDirection();
		float a = dir.Length()*dir.Length(), b = dir.Dot3(origin) * 2, c = origin.Dot3(origin) - radius * radius;
		float d = b * b - 4 * a*c;
		float t = 0;
		if (d < 0) return false;
		if (d == 0) {
			t = -b / (2 * a);
		}
		else {
			d = sqrt(d);
			t = (-b - d) / (2 * a);
			if (t < tmin) t = (-b + d) / (2 * a);
		}
		if (t > tmin) {
			if (h.getT() > t) {
				Vec3f hit_point = r.pointAtParameter(t);
				Vec3f normal = hit_point - center;
				normal.Normalize();
				h.set(t, material, normal, r);
				return true;
			}
		}
		return false;
	}
	virtual void paint() {
		if (material) material->glSetMaterial();
		glBegin(GL_QUADS);
		for (float iPhi = -90; iPhi < 90; iPhi += phi_step) {
			for (float iTheta = 0; iTheta < 360; iTheta += theta_step) {
				// compute appropriate coordinates & normals
				Vec3f x0 = pt2xyz(iPhi, iTheta);
				Vec3f n0 = pt2Normal(iPhi, iTheta);
				Vec3f x1 = pt2xyz(iPhi + phi_step, iTheta);
				Vec3f n1 = pt2Normal(iPhi + phi_step, iTheta);
				Vec3f x2 = pt2xyz(iPhi + phi_step, iTheta + theta_step);
				Vec3f n2 = pt2Normal(iPhi + phi_step, iTheta + theta_step);
				Vec3f x3 = pt2xyz(iPhi, iTheta + theta_step);
				Vec3f n3 = pt2Normal(iPhi, iTheta + theta_step);

				// send gl vertex commands
				Vec3f n;
				Vec3f::Cross3(n, x2 - x0, x1 - x0);
				glNormal3f(n.x(), n.y(), n.z());
				if(gouraud) glNormal3f(n0.x(), n0.y(), n0.z());
				glVertex3f(x0.x(), x0.y(), x0.z());
				if(gouraud)  glNormal3f(n1.x(), n1.y(), n1.z());
				glVertex3f(x1.x(), x1.y(), x1.z());
				if (gouraud)  glNormal3f(n2.x(), n2.y(), n2.z());
				glVertex3f(x2.x(), x2.y(), x2.z());
				if (gouraud)  glNormal3f(n3.x(), n3.y(), n3.z());
				glVertex3f(x3.x(), x3.y(), x3.z());
			}
		}
		glEnd();
	}
};