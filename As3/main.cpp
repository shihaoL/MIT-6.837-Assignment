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
#include "RayTracer.h"
#include "rayTree.h"
#include "film.h"
#include "Filter.h"
#include "Sampler.h"
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
bool shadows = false;
int bounces = 0;
float weight = 0;
bool visual_grid = false;
bool use_grid = false;
int nx, ny, nz;
bool stats = false;
bool use_render_samples = false;
char* render_sample_name = NULL;
int render_sample_zoom_factor = 0;
bool box_filter = false;
float box_radius = 0.0f;
bool tent_filter = false;
float tent_radius = 0.0f;
bool gaussian_filter = false;
float gaussian_sigma = 0.0f;
bool use_render_filter = false;
char* render_filter_name = NULL;
int render_filter_zoom_factor=0;
int num_samples = 1;
bool uniform_samples = false;
bool jitter_samples = false;
bool random_samples = false;


SceneParser* scene = NULL;
RayTracer* tracer = NULL;

void render();
void debugTracerRay(float x, float y) {
	Hit h(INFINITY, NULL, { 0,0,1 });
	auto cam = scene->getCamera();
	Ray r = cam->generateRay({ x,y });
	tracer->traceRay(r, cam->getTMin(), 0, 1, 1, h);
	RayTree::SetMainSegment(r, 0, h.getT());
}
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
		else if (!strcmp(argv[i], "-shadows")) {
			shadows = true;
		}
		else if (!strcmp(argv[i], "-bounces")) {
			i++; assert(i < argc);
			bounces = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight")) {
			i++; assert(i < argc);
			weight = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-grid")) {
			use_grid = true;
			i++; assert(i < argc);
			nx = atoi(argv[i]);
			i++; assert(i < argc);
			ny = atoi(argv[i]);
			i++; assert(i < argc);
			nz = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-visualize_grid")) {
			visual_grid = true;
		}
		else if (!strcmp(argv[i], "-stats")) {
			stats = true;
		}
		else if (!strcmp(argv[i], "-render_samples")) {
			use_render_samples = true;
			i++; assert(i < argc);
			render_sample_name = argv[i];
			i++; assert(i < argc);
			render_sample_zoom_factor = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-render_filter")) {
			use_render_filter = true;
			i++; assert(i < argc);
			render_filter_name = argv[i];
			i++; assert(i < argc);
			render_filter_zoom_factor = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-box_filter")) {
			box_filter = true;
			i++; assert(i < argc);
			box_radius = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-tent_filter")) {
			tent_filter = true;
			i++; assert(i < argc);
			tent_radius = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-gaussian_filter")) {
			gaussian_filter = true;
			i++; assert(i < argc);
			gaussian_sigma = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-random_samples")) {
			random_samples = true;
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-jittered_samples")) {
			jitter_samples = true;
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-uniform_samples")) {
			uniform_samples = true;
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	scene = new SceneParser(input_file);
	tracer = new RayTracer(scene, bounces, weight, shadows, shade_back, visual_grid, use_grid, nx, ny, nz);
	if (gui) {
		glutInit(&argc, argv);
		GLCanvas glcanvas;
		glcanvas.initialize(scene,render,debugTracerRay,tracer->grid,visual_grid);
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
	Camera* cam = scene->getCamera();
	RayTracingStats::Initialize(width,height,scene->getGroup()->getBoundingBox(),
		nx,ny,nz);
	Film film(width, height, num_samples);
	shared_ptr<Sampler> sampler = nullptr;
	using sampler_ptr = shared_ptr<Sampler>;
	if (uniform_samples) {
		sampler = sampler_ptr(new UniformSampler(num_samples));
	}
	else if (random_samples) {
		sampler = sampler_ptr(new RandomSampler(num_samples));
	}
	else if (jitter_samples) {
		sampler = sampler_ptr(new JitteredSampler(num_samples));
	}
	else {
		sampler = sampler_ptr(new UniformSampler(1));
	}
	shared_ptr<Filter> filter = nullptr;
	using filter_ptr = shared_ptr<Filter>;
	if (box_filter) {
		filter = filter_ptr(new BoxFilter(box_radius));
	}
	else if (tent_filter) {
		filter = filter_ptr(new TentFilter(tent_radius));
	}
	else if (gaussian_filter) {
		filter = filter_ptr(new GaussianFilter(gaussian_sigma));
	}
	else {
		filter = filter_ptr(new BoxFilter(0.001));
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			for (int k = 0; k < num_samples; k++) {
				auto p = sampler->getSamplePosition(k);
				float x = i + p.x(), y = j + p.y();
				Hit h(INFINITY, NULL, { 0,0,1 });
				Ray r = cam->generateRay({ x / width,y / height });
				RayTracingStats::IncrementNumNonShadowRays();
				auto color = tracer->traceRay(r, cam->getTMin(), 0, 1, 1, h);
				film.setSample(i, j, k, p, color);
				
			}
			
		}
	}
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (color_img)
				color_img->SetPixel(i, j, filter->getColor(i, j, &film));
		}
	}
	if (use_render_samples) {
		film.renderSamples(render_sample_name, render_sample_zoom_factor);
	}
	if (use_render_filter) {
		film.renderFilter(render_filter_name, render_filter_zoom_factor, filter.get());
	}
	if (color_img) color_img->SaveTGA(output_file);
	if (depth_img) depth_img->SaveTGA(depth_file);
	if (normal_img) normal_img->SaveTGA(normals_file);
	delete color_img;
	delete depth_img;
	delete normal_img;
	printf("render ok\n");
	if (stats) {
		RayTracingStats::PrintStatistics();
	}
}