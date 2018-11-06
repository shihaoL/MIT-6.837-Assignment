#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "image.h"
#include "scene_parser.h"
#include "hit.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include "group.h"
#include "glCanvas.h"
#include "GL/freeglut.h"
using namespace std;



void diffuse_shader(const SceneParser& scene, const Hit& hit, Vec3f& color,const Ray& ray) {
	auto ambient = scene.getAmbientLight();
	Vec3f res = ambient * (hit.getMaterial()->getDiffuseColor());
	for (int i = 0; i < scene.getNumLights(); i++) {
		auto light = scene.getLight(i);
		Vec3f light_dir, light_col;
		float distanceToLight;
		light->getIllumination(hit.getIntersectionPoint(), light_dir, light_col,distanceToLight);
		res += hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
	}
	color = res;
}

void normal_shader(const Hit& hit, Vec3f& color) {
	color.Set(abs(hit.getNormal().x()), abs(hit.getNormal().y()), abs(hit.getNormal().z()));
}


void depth_shader(const Hit& h,float d_max,float d_min, Vec3f& color) {
	float t = h.getT();
	float gray_value = 1 - (t - d_min) / (d_max- d_min);
	color.Set(gray_value, gray_value, gray_value);
}

float phi_step=10,theta_step=10;
bool gouraud=false;
char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char *normals_file = NULL;
bool shade_back = false;
bool gui = false;
SceneParser* scene = NULL;


void render();
int main(int argc, char* argv[]) {
	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			width = atoi(argv[i]);
			i++; assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-normals")) {
			i++; assert(i < argc);
			normals_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back")) {
			shade_back = true;
		}
		else if (!strcmp(argv[i], "-tessellation")) {
			i++; assert(i < argc);
			theta_step = atof(argv[i]);
			theta_step = 360 / theta_step;
			i++; assert(i < argc);
			phi_step = atof(argv[i]);
			phi_step = 180 / phi_step;
		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud = true;
		}
		else if (!strcmp(argv[i], "-gui")) {
			gui = true;
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	scene = new SceneParser(input_file);
	if (gui) {
		glutInit(&argc, argv);
		GLCanvas glcanvas;
		glcanvas.initialize(scene,render);
	}
	else {
		render();
	}
	delete scene;
	// ========================================================
	// ========================================================
}

void render() {
	Image *color_img = NULL, *depth_img = NULL, *normal_img = NULL;
	if (output_file) {
		color_img = new Image(width, height);
		color_img->SetAllPixels(scene->getBackgroundColor());
	}
	if (depth_file) {
		depth_img = new Image(width, height);
		depth_img->SetAllPixels({ 0,0,0 });
	}
	if (normals_file) {
		normal_img = new Image(width, height);
		normal_img->SetAllPixels({ 0,0,0 });
	}
	Group* g = scene->getGroup();
	Camera* cam = scene->getCamera();
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			Hit h(INFINITY, NULL, { 0,0,1 });
			Ray r = cam->generateRay({ i*1.0f / width,j*1.0f / width });
			bool hited = g->intersect(r, h, cam->getTMin());
			if (hited) {
				Vec3f hit_normal = h.getNormal();
				bool back = hit_normal.Dot3(r.getDirection()) > 0;
				if (back&&shade_back == false) {
					if (color_img)
						color_img->SetPixel(i, j, { 0,0,0 });
					if (depth_img)
						depth_img->SetPixel(i, j, { 0,0,0 });
					if (normal_img)
						normal_img->SetPixel(i, j, { 0,0,0 });
					continue;
				}
				if (back) hit_normal.Negate();
				h.set(h.getT(), h.getMaterial(), hit_normal, r);
				Vec3f pixel_color;
				if (color_img) {
					diffuse_shader(*scene, h, pixel_color, r);
					color_img->SetPixel(i, j, pixel_color);
				}
				if (depth_img) {
					depth_shader(h, depth_max, depth_min, pixel_color);
					depth_img->SetPixel(i, j, pixel_color);
				}
				if (normal_img) {
					normal_shader(h, pixel_color);
					normal_img->SetPixel(i, j, pixel_color);
				}
			}
		}
	}
	if (color_img) color_img->SaveTGA(output_file);
	if (depth_img) depth_img->SaveTGA(depth_file);
	if (normal_img) normal_img->SaveTGA(normals_file);
	delete color_img;
	delete depth_img;
	delete normal_img;
	printf("render ok\n");
}