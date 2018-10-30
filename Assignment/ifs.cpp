#include "ifs.h"
#include <cstdlib>
#include <ctime>
using namespace std;

int IFS::rand_pick()
{
	float p = rand()*1.0 / RAND_MAX;
	for (int i = 0; i < probabilities.size(); i++) {
		if (p < probabilities[i]) {
			return i;
		}
		p -= probabilities[i];
	}
	return 0;
}
void IFS::render(Image * img, int n, int iter)
{
	srand(time(NULL));
	int x_max = img->Width(), y_max = img->Height();
	for (int i = 0; i < n; i++) {
		float nx = rand()*1.0/RAND_MAX, ny = rand()*1.0/RAND_MAX;
		Vec2f res{nx,ny};
		for (int j = 0; j < iter; j++) {
			int idx = rand_pick();
			matries[idx].Transform(res);
		}
		img->SetPixel((int)(x_max*res.x()), (int)(y_max*res.y()), { 1,1,1 });
	}
}
