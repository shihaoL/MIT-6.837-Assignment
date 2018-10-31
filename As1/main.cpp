// ========================================================
// ========================================================
// Some sample code you might like to use for parsing 
// command line arguments 
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "image.h"
#include "scene_parser.h"
using namespace std;
int main(int argc, char* argv[]) {

	char *input_file = NULL;
	int width = 100;
	int height = 100;
	char *output_file = NULL;
	float depth_min = 0;
	float depth_max = 1;
	char *depth_file = NULL;

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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	Image img(width, height);
	Image gray(width, height);
	SceneParser scence(input_file);
	Group* g = scence.getGroup();
	Camera* cam = scence.getCamera();
	for (int i = 0; i <width; i ++) {
		for (int j = 0; j < height;j++) {
			Hit h(INFINITY,NULL);
			Ray r = cam->generateRay({ i*1.0f/width,j*1.0f/width });
			bool hited = g->intersect(r, h, cam->getTMin());
			if (hited) {
				img.SetPixel(i,j,h.getMaterial()->getDiffuseColor());
				float t = h.getT();
				float gray_value = 1- (t - depth_min) / (depth_max - depth_min);
				gray.SetPixel(i,j, { gray_value,gray_value,gray_value });
			}
		}
	}
	img.SaveTGA(output_file);
	gray.SaveTGA(depth_file);
	printf("hello world as1");
	// ========================================================
	// ========================================================
}