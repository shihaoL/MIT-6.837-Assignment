#pragma once
#include "arg_parser.h"
#include "triangle_mesh.h"
#include "vectors.h"
#include <vector>
#include "matrix.h"
#include <GL/GL.h>
#include <GL/GLU.h>
class Spline {
protected:
	std::vector<Vec3f> points;
public:
	void set(int i, Vec3f p) {
		points[i] = p;
	}
	// FOR VISUALIZATION
	virtual void Paint(ArgParser *args) {

	}

	// FOR CONVERTING BETWEEN SPLINE TYPES
	virtual void OutputBezier(FILE *file) {
	}
	virtual void OutputBSpline(FILE *file) {

	}

	// FOR CONTROL POINT PICKING
	virtual int getNumVertices() {
		return points.size();
	}
	virtual Vec3f getVertex(int i) {
		return points[i];
	}

	// FOR EDITING OPERATIONS
	virtual void moveControlPoint(int selectedPoint, float x, float y) {
		points[selectedPoint].Set(x, y, 0);
	}
	virtual void addControlPoint(int selectedPoint, float x, float y) {
		points.insert(points.begin() + selectedPoint, { x,y,0 });
	}
	virtual void deleteControlPoint(int selectedPoint) {
		points.erase(points.begin() + selectedPoint);
	}

	// FOR GENERATING TRIANGLES
	virtual TriangleMesh* OutputTriangles(ArgParser *args) {
		return NULL;
	}
};

class Curve:public Spline {
protected:
	virtual void paint_curve(int n)=0;
public:
	//virtual Vec3f get_interpolation(float t) = 0;
	virtual vector<Vec3f> get_points(int n) = 0;
	virtual void Paint(ArgParser *args) {
		int n = args->curve_tessellation;
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(1, 0, 0);
		for (auto& p : points) {
			glVertex3f(p.x(), p.y(), p.z());
		}
		glEnd();
		glLineWidth(2);
		glColor3f(0, 0, 1);
		glBegin(GL_LINE_STRIP);
		for (auto& p : points) {
			glVertex3f(p.x(), p.y(), p.z());
		}
		glEnd();
		paint_curve(n);
	}
};

class BezierCurve :public Curve {
public:
	BezierCurve(int num_vertex):num_vertex(num_vertex){
		points.resize(num_vertex);
	}
	virtual void OutputBezier(FILE *file) {
		fprintf(file, "bezier\nnum_vertices %d\n",points.size());
		for (int i = 0; i < points.size(); i++) {
			fprintf(file, "%.2f %.2f %.2f\n", points[i].x(), points[i].y(), points[i].z());
		}
	}
	virtual void OutputBSpline(FILE *file) {
		float bs[] = {
			-1,3,-3,1,
			3,-6,0,4,
			-3,3,3,1,
			1,0,0,0
		};
		auto Bs = Matrix(
			bs
		);
		Bs = 1.0f / 6 * Bs;
		float b[] = {
			-1,3,-3,1,
			3,-6,3,0,
			-3,3,0,0,
			1,0,0,0
		};
		auto B = Matrix(b);
		Bs.Inverse();
		Matrix G;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				G.Set(j, i, points[j][i]);
			}
		}
		for (int i = 0; i < 4; i++) G.Set(i, 3, 1);
		G=G*B*Bs;
		fprintf(file, "bspline\nnum_vertices 4\n");
		for (int i = 0; i < 4; i++) {
			fprintf(file, "%.2f %.2f %.2f\n", G.Get(i, 0), G.Get(i, 1), G.Get(i, 2));
		}
	}
	void addControlPoint(int selectedPoint, float x, float y) {

	}
	void deleteControlPoint(int selectedPoint) {
		//points.erase(points.begin() + selectedPoint);
	}
	//virtual Vec3f get_interpolation(float t) {
	//	float ca = (1 - t)*(1 - t)*(1 - t);
	//	float cb = 3 * t*(1 - t)*(1 - t);
	//	float cc = 3 * t*t*(1 - t);
	//	float cd = t * t*t;
	//	Vec3f p = ca * a + cb * b + cc * c + cd * d;
	//}
	virtual vector<Vec3f> get_points(int n) {
		vector<Vec3f> res;
		for (int i = 0; 3 * i < num_vertex - 1; i++) {
			auto a = points[3 * i], b = points[3 * i + 1], c = points[3 * i + 2], d = points[3 * i + 3];
			float dt = 1.0f / (n - 1);
			float t = 0;
			for (int j = 0; j < n; j++) {
				float ca = (1 - t)*(1 - t)*(1 - t);
				float cb = 3 * t*(1 - t)*(1 - t);
				float cc = 3 * t*t*(1 - t);
				float cd = t * t*t;
				Vec3f p = ca * a + cb * b + cc * c + cd * d;
				res.push_back(p);
				t += dt;
			}
		}
		return res;
	}
private:
	int num_vertex;
protected:
	void paint_4(Vec3f a, Vec3f b, Vec3f c, Vec3f d, int n) {
		float dt = 1.0f / (n - 1);
		glLineWidth(5);
		glColor3f(0, 1, 0);
		float t = 0;
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < n; i++) {
			float ca = (1 - t)*(1 - t)*(1 - t);
			float cb = 3 * t*(1 - t)*(1 - t);
			float cc = 3 * t*t*(1 - t);
			float cd = t * t*t;
			Vec3f p = ca * a + cb * b + cc * c + cd * d;
			glVertex3f(p.x(), p.y(), p.z());
			t += dt;
		}
		glEnd();
	}
	virtual void paint_curve(int n) {
		//float d = 1.0f / (n - 1);
		for (int i = 0; 3 * i < num_vertex - 1; i++) {
			paint_4(points[3 * i], points[3 * i + 1], points[3 * i + 2], points[3 * i + 3], n);
		}
	}
};

class BSplineCurve :public Curve {
private:
	int num_vertex;
public:
	BSplineCurve(int num_vertex):num_vertex(num_vertex){
		points.resize(num_vertex);
	}
	virtual void OutputBezier(FILE *file) {
		float bs[] = {
			-1,3,-3,1,
			3,-6,0,4,
			-3,3,3,1,
			1,0,0,0
		};
		auto Bs = Matrix(
			bs
		);
		Bs = 1.0f / 6 * Bs;
		float b[] = {
			-1,3,-3,1,
			3,-6,3,0,
			-3,3,0,0,
			1,0,0,0
		};
		auto B = Matrix(b);
		B.Inverse();
		Matrix G;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				G.Set(j, i, points[j][i]);
			}
		}
		for (int i = 0; i < 4; i++) G.Set(i, 3, 1);
		G = G *Bs*B;
		fprintf(file, "bezier\nnum_vertices 4\n");
		for (int i = 0; i < 4; i++) {
			fprintf(file, "%.2f %.2f %.2f\n", G.Get(i, 0), G.Get(i, 1), G.Get(i, 2));
		}
	}
	virtual void OutputBSpline(FILE *file) {
		fprintf(file, "bspline\nnum_vertices %d\n",points.size());
		for (int i = 0; i < points.size(); i++) {
			fprintf(file, "%.2f %.2f %.2f\n", points[i].x(), points[i].y(), points[i].z());
		}
	}
	void addControlPoint(int selectedPoint, float x, float y) {

	}
	void deleteControlPoint(int selectedPoint) {
		//points.erase(points.begin() + selectedPoint);
	}
	virtual vector<Vec3f> get_points(int n) {
		vector<Vec3f> res;
		for (int i = 0;  i+3 < num_vertex; i++) {
			auto a = points[i], b = points[i + 1], c = points[i + 2], d = points[i + 3];
			float dt = 1.0f / (n - 1);
			float t = 0;
			for (int j = 0; j < n; j++) {
				float ca = (1 - t)*(1 - t)*(1 - t);
				float cb = (3 * t*t*t - 6 * t*t + 4);
				float cc = -3 * t*t*t + 3 * t*t + 3 * t + 1;
				float cd = t * t*t;
				Vec3f p = ca / 6 * a + cb / 6 * b + cc / 6 * c + cd / 6 * d;
				res.push_back(p);
				t += dt;
			}
		}
		return res;
	}
protected:
	void paint_4(Vec3f a, Vec3f b, Vec3f c, Vec3f d, int n) {
		float dt = 1.0f / (n - 1);
		glLineWidth(5);
		glColor3f(0, 1, 0);
		float t = 0;
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < n; i++) {
			float ca = (1 - t)*(1 - t)*(1 - t);
			float cb = (3 * t*t*t-6*t*t+4);
			float cc = -3 * t*t*t+3*t*t+3*t+1;
			float cd = t * t*t;
			Vec3f p = ca/6 * a + cb/6 * b + cc/6 * c + cd/6 * d;
			glVertex3f(p.x(), p.y(), p.z());
			t += dt;
		}
		glEnd();
	}
	virtual void paint_curve(int n) {
		for (int i = 0; i+3 < num_vertex; i++) {
			paint_4(points[i], points[i + 1], points[i + 2], points[i + 3], n);
		}
	}
};

class Surface :public Spline {

};

class SurfaceOfRevolution :public Surface {
private:
	Curve * c;
public:
	SurfaceOfRevolution(Curve* c):c(c){

	}
	virtual TriangleMesh* OutputTriangles(ArgParser *args) {
		int u = args->curve_tessellation, v = args->revolution_tessellation;
		auto vs = c->get_points(u);
		u = vs.size()-1;
		TriangleNet* t=new TriangleNet(u, v);
		for (int i = 0; i <= v; i++) {
			auto m = Matrix::MakeYRotation(i*1.0f / v * 2 * 3.14159);
			for (int j = 0; j <= u; j++) {
				auto p = vs[j];
				m.Transform(p);
				t->SetVertex(j, i, p);
			}
		}
		return t;
	}
};
class BezierPatch :public Surface {
private:
	Vec3f help_bezier(Vec3f a, Vec3f b, Vec3f c, Vec3f d, float t) {
		float ca = (1 - t)*(1 - t)*(1 - t);
		float cb = 3 * t*(1 - t)*(1 - t);
		float cc = 3 * t*t*(1 - t);
		float cd = t * t*t;
		Vec3f p = ca * a + cb * b + cc * c + cd * d;
		return p;
	}
public:
	BezierPatch() {
		points.resize(16);
	}
	virtual TriangleMesh* OutputTriangles(ArgParser *args) {
		int n = args->patch_tessellation;
		TriangleNet *tn = new TriangleNet(n, n);
		float dts = 1.0f / n;
		float t = 0;
		float s = 0;
		for (int i = 0; i <= n; i++) {
			Vec3f p[4];
			for (int j = 0; j < 4; j++) {
				p[j] = help_bezier(points[4 * j], points[4 * j + 1], points[4 * j+ 2], points[4 * j + 3], t);
			}
			s = 0;
			for (int j = 0; j <= n; j++) {
				auto pp = help_bezier(p[0], p[1], p[2], p[3], s);
				tn->SetVertex(i, j, pp);
				s += dts;
			}
			t += dts;
		}
		return tn;
	}
};

