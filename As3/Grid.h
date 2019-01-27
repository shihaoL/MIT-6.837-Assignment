#pragma once
#include "object3d.h"
#include "rayTree.h"
#include "matrix.h"
#include <vector>

struct MarchingInfo {
	bool hit;
	float tmin;
	float t_next[3];
	int idx[3];
	float d[3];
	int sign[3];
	int hit_axis=0;
	void nextCell() {
		int min_idx = -1;
		float min_t = INFINITY;
		for (int i = 0; i < 3; i++) {
			if (t_next[i] < min_t) {
				min_t = t_next[i];
				min_idx = i;
			}
		}
		idx[min_idx] += sign[min_idx];
		tmin = t_next[min_idx];
		t_next[min_idx] += d[min_idx];
		hit_axis = min_idx;
	}
	bool is_end(int nx, int ny, int nz) {
		if (idx[0] >= 0 && idx[0] < nx
			&&idx[1] >= 0 && idx[1] < ny
			&&idx[2] >= 0 && idx[2] < nz) {
			return false;
		}
		return true;
	}
};

class Grid :
	public Object3D
{
private:
	float dx, dy, dz;
	std::vector<vector<Object3D*>> opaque;
	void setBoundingBox()override {
	}
public:
	void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const {
		auto dir = r.getDirection();
		auto origin = r.getOrigin();
		auto bmin = bb->getMin();
		auto bmax = bb->getMax();
		//inside
		if (origin.x() > bmin.x() && origin.x() < bmax.x()
			&& origin.y() > bmin.y() && origin.y() < bmax.y()
			&& origin.z() > bmin.z() && origin.z() < bmax.z()) {
			int x = floor((origin.x() - bmin.x()) / dx);
			int y = floor((origin.y() - bmin.y()) / dy);
			int z = floor((origin.z() - bmin.z()) / dz);
			bmin = Vec3f(x*dx + bmin.x(), y*dy + bmin.y(), z*dz + bmin.z());
			bmax = Vec3f(bmin.x() + dx, bmin.y() + dy, bmin.z() + dz);
			mi.tmin = tmin;
			mi.hit = true;
			for (int i = 0; i < 3; i++) {
				float t1 = (bmin[i] - origin[i]) / dir[i];  
				float t2 = (bmax[i] - origin[i]) / dir[i];
				if (t1 > t2) swap(t1, t2);
				mi.t_next[i] = t2;
			}
			mi.idx[0] = x;
			mi.idx[1] = y;
			mi.idx[2] = z;
		}
		else {
			if (dir.x() == 0 && (origin.x() < bmin.x() || origin.x() > bmax.x())
				|| dir.y() == 0 && (origin.y() < bmin.y() || origin.y() > bmax.y())
				|| dir.z() == 0 && (origin.z() < bmin.z() || origin.z() > bmax.z())
				) {
				mi.hit = false;
				return;
			}
			float tnear = -INFINITY, tfar = INFINITY;
			int tmp_axis = 0;
			for (int i = 0; i < 3; i++) {
				float t1 = (bmin[i] - origin[i]) / dir[i];
				float t2 = (bmax[i] - origin[i]) / dir[i];
				if (t1 > t2) swap(t1, t2);
				if (t1 > tnear) {
					tnear = t1; tmp_axis = i;
				}
				if (t2 < tfar) tfar = t2;
			}
			if (tnear > tfar || tfar < tmin) {
				mi.hit = false;
				return;
			}
			//outside hit
			else {
				mi.hit = true;
				mi.tmin = tnear;
				auto p = tnear * dir + origin;
				int x = floor((p.x() - bmin.x()) / dx);
				if (x < 0) x = 0;
				if (x >= nx) x = nx - 1;
				int y = floor((p.y() - bmin.y()) / dy);
				if (y < 0) y = 0;
				if (y >= ny) y = ny - 1;
				int z = floor((p.z() - bmin.z()) / dz);
				if (z < 0) z = 0;
				if (z >= nz) z = nz - 1;
				bmin = Vec3f(x*dx+bmin.x(), y*dy+bmin.y(), z*dz+bmin.z());
				bmax = Vec3f(bmin.x()+dx, bmin.y()+dy, bmin.z()+dz);
				for (int i = 0; i < 3; i++) {
					float t1 = (bmin[i] - p[i]) / dir[i];
					float t2 = (bmax[i] - p[i]) / dir[i];
					if (t1 > t2) swap(t1, t2);
					mi.t_next[i] = t2+tnear;
				}
				mi.idx[0] = x;
				mi.idx[1] = y;
				mi.idx[2] = z;
				mi.hit_axis = tmp_axis;
			}
		}
		for (int i = 0; i < 3; i++) {
			if (dir[i] > 0) mi.sign[i] = 1;
			else mi.sign[i] = -1;
		}
		mi.d[0] = abs(dx / dir.x());
		mi.d[1] = abs(dy / dir.y());
		mi.d[2] = abs(dz / dir.z());
	}
	virtual bool intersect(const Ray &r, Hit &h, float tmin) {
		MarchingInfo mi;
		initializeRayMarch(mi, r, tmin);
		if (mi.hit == false) return false;
		while (!mi.is_end(nx, ny, nz)) {
			int  x = mi.idx[0], y = mi.idx[1], z = mi.idx[2];
			if (checkOpaque(x,y,z)) {
				{
					float xx = x * dx + bb->getMin().x();
					float yy = y * dy + bb->getMin().y();
					float zz = z * dz + bb->getMin().z();
					RayTree::AddHitCellFace({ xx,yy,zz }, { xx + dx,yy,zz }, { xx + dx, yy + dy, zz }, { xx, yy + dy, zz }, { 0, 0, -1 },&RED);
					RayTree::AddHitCellFace({ xx,yy,zz }, { xx + dx, yy, zz }, { xx + dx, yy, zz + dz }, { xx, yy, zz + dz }, { 0, -1, 0 },&RED);
					RayTree::AddHitCellFace({ xx, yy + dy, zz }, { xx + dx, yy + dy, zz }, { xx + dx, yy + dy, zz + dz }, { xx, yy + dy, zz + dz }, { 0, 1, 0 },&RED);
					RayTree::AddHitCellFace({ xx, yy, zz }, { xx, yy + dy, zz }, { xx, yy + dy, zz + dz }, { xx, yy, zz + dz }, { -1, 0, 0 },&RED);
					RayTree::AddHitCellFace({ xx + dx, yy, zz }, { xx + dx, yy + dy, zz }, { xx + dx, yy + dy, zz + dz }, { xx + dx, yy, zz + dz }, { 1, 0, 0 },&RED);
					RayTree::AddHitCellFace({ xx, yy, zz + dz }, { xx + dx, yy, zz + dz }, { xx + dx, yy + dy, zz + dz }, { xx, yy + dy, zz + dz }, { 0, 0, 1 },&RED);
				}
				float normal[3] = { 0,0,0 };
				normal[mi.hit_axis] = 1;
				Vec3f nn(normal[0], normal[1], normal[2]);
				if (nn.Dot3(r.getDirection()) > 0) nn.Negate();
				h.set(mi.tmin, &WHITE, nn, r);
				return true;
			}
			mi.nextCell();
		}
		return false;
	}
	virtual bool intersectShadowRay(const Ray &r, Hit &h, float tmin) {
		return intersect(r, h, tmin);
	}
	void setOpaque(int x, int y, int z, Object3D* value) {
		opaque[z*(nx*ny) + y * nx + x].push_back(value);
	}
	bool checkOpaque(int x, int y, int z) {
		return !opaque[z*(nx*ny) + y * nx + x].empty();
	}
	vector<Object3D*>& getOpaque(int x, int y, int z) {
		return opaque[z*(nx*ny) + y * nx + x];
	}
	int nx, ny, nz;
	Grid(BoundingBox *bb, int nx, int ny, int nz):Object3D(&WHITE),nx(nx),ny(ny),nz(nz) {
		this->bb = bb;
		opaque.resize(nx*ny*nz);
		auto bmax = bb->getMax();
		auto bmin = bb->getMin();
		dx = (bmax.x() - bmin.x()) / nx;
		dy = (bmax.y() - bmin.y()) / ny;
		dz = (bmax.z() - bmin.z()) / nz;
	}
	virtual ~Grid() {}
	void paint() {
		auto tmin = bb->getMin();
		auto tmax = bb->getMax();
		glBegin(GL_QUADS);
		for (int x = 0; x < nx; x++) {
			for (int y = 0; y < ny; y++) {
				for (int z = 0; z < nz; z++) {
					if (!checkOpaque(x, y, z)) continue;
					float xx = x * dx + tmin.x();
					float yy = y * dy + tmin.y();
					float zz = z * dz + tmin.z();
					{
						glNormal3f(0, 0, -1);
						glVertex3f(xx, yy, zz);
						glVertex3f(xx+dx, yy, zz);
						glVertex3f(xx+dx, yy+dy, zz);
						glVertex3f(xx, yy+dy, zz);
					}
					{
						glNormal3f(0, 0, 1);
						glVertex3f(xx, yy, zz+dz);
						glVertex3f(xx + dx, yy, zz+dz);
						glVertex3f(xx + dx, yy + dy, zz+dz);
						glVertex3f(xx, yy + dy, zz+dz);
					}
					{
						glNormal3f(1, 0, 0);
						glVertex3f(xx + dx, yy, zz);
						glVertex3f(xx + dx, yy + dy, zz);
						glVertex3f(xx + dx, yy + dy, zz + dz);
						glVertex3f(xx + dx, yy, zz + dz);
					}
					{
						glNormal3f(-1, 0, 0);
						glVertex3f(xx, yy, zz);
						glVertex3f(xx, yy + dy, zz);
						glVertex3f(xx, yy + dy, zz + dz);
						glVertex3f(xx, yy, zz + dz);
					}
					{
						glNormal3f(0, 1, 0);
						glVertex3f(xx, yy+dy, zz);
						glVertex3f(xx + dx, yy + dy, zz);
						glVertex3f(xx + dx, yy + dy, zz + dz);
						glVertex3f(xx, yy+dy, zz + dz);
					}
					{
						glNormal3f(0, -1, 0);
						glVertex3f(xx, yy, zz);
						glVertex3f(xx + dx, yy, zz);
						glVertex3f(xx + dx, yy, zz + dz);
						glVertex3f(xx, yy, zz + dz);
					}
				}
			}
		}
		glEnd();
	}
	void intoGrid(Vec3f tmin,Vec3f tmax,Object3D* value) {
		auto bmin = bb->getMin();
		auto bmax = bb->getMax();
		int xmin = floor((tmin.x() - bmin.x()) / (bmax.x() - bmin.x())*nx);
		if (xmin == nx) xmin = nx - 1;
		int xmax = floor((tmax.x() - bmin.x()) / (bmax.x() - bmin.x())*nx);
		if (xmax == nx) xmax = nx - 1;
		int ymin = floor((tmin.y() - bmin.y()) / (bmax.y() - bmin.y())*ny);
		if (ymin == ny) ymin = ny - 1;
		int ymax = floor((tmax.y() - bmin.y()) / (bmax.y() - bmin.y())*ny);
		if (ymax == ny) ymax = ny - 1;
		int zmin = floor((tmin.z() - bmin.z()) / (bmax.z() - bmin.z())*nz);
		if (zmin == nz) zmin = nz - 1;
		int zmax = floor((tmax.z() - bmin.z()) / (bmax.z() - bmin.z())*nz);
		if (zmax == nz) zmax = nz - 1;
		for (int x = xmin; x <= xmax; x++) {
			for (int y = ymin; y <= ymax; y++) {
				for (int z = zmin; z <= zmax; z++) {
					setOpaque(x, y, z, value);
				}
			}
		}
	}
	void transform_into_Grid(BoundingBox* t_bb, Matrix* _m,Object3D* value) {
		auto t_max = t_bb->getMax(), t_min = t_bb->getMin();
		Vec3f p[8] = {
			{ t_max[0],t_max[1],t_max[2] },
		{ t_max[0],t_max[1],t_min[2] },
		{ t_max[0],t_min[1],t_max[2] },
		{ t_max[0],t_min[1],t_min[2] },
		{ t_min[0],t_max[1],t_max[2] },
		{ t_min[0],t_max[1],t_min[2] },
		{ t_min[0],t_min[1],t_max[2] },
		{ t_min[0],t_min[1],t_min[2] }
		};
		float maxx = -INFINITY, maxy = -INFINITY, maxz = -INFINITY, minx = INFINITY, miny = INFINITY, minz = INFINITY;
		for (int i = 0; i < 8; i++) {
			_m->Transform(p[i]);
			if (p[i].x() > maxx) maxx = p[i].x();
			if (p[i].x() < minx) minx = p[i].x();
			if (p[i].y() > maxy) maxy = p[i].y();
			if (p[i].y() < miny)miny = p[i].y();
			if (p[i].z() > maxz) maxz = p[i].z();
			if (p[i].z() < minz) minz = p[i].z();
		}
		intoGrid(Vec3f(minx, miny, minz), Vec3f(maxx, maxy, maxz),value);
	}
};

