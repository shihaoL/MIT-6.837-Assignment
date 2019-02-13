#pragma once
#include "vectors.h"
#include "film.h"
class Filter {
public:
	Vec3f getColor(int i, int j, Film* film) {
		int radius = getSupportRadius();
		int w = film->getWidth(), h = film->getHeight();
		float cx = i + 0.5, cy = j + 0.5;
		int mini = i - radius, maxi = i + radius;
		if (mini < 0) mini = 0;
		if (maxi >= w) maxi = w - 1;
		int minj = j - radius, maxj = j + radius;
		if (minj < 0) minj = 0;
		if (maxj >= h) maxj = h - 1;
		int n = film->getNumSamples();
		float total_weight = 0.0f;
		Vec3f total_color(0, 0, 0);
		for (int i = mini; i <= maxi; i++) {
			for (int j = minj; j <= maxj; j++) {
				for (int k = 0; k < n; k++) {
					auto s = film->getSample(i, j, k);
					float tx = i + s.getPosition().x(), ty = j + s.getPosition().y();
					float w = getWeight(tx - cx, ty - cy);
					total_color += w * s.getColor();
					total_weight += w;
				}
			}
		}
		return 1 / total_weight * total_color;
	}
	virtual float getWeight(float x, float y) = 0;
	virtual int getSupportRadius() = 0;
};

class BoxFilter :public Filter {
private:
	float radius;
public:
	BoxFilter(float radius):radius(radius){}
	virtual float getWeight(float x, float y) {
		if (abs(x) <= radius && abs(y) <= radius) return 1.0f;
		else return 0.0f;
	}
	virtual int getSupportRadius() {
		return floor(radius + 0.4999);
	}
};

class TentFilter :public Filter {
private:
	float radius;
public:
	TentFilter(float radius):radius(radius){}
	virtual float getWeight(float x, float y) {
		float d = sqrt(x*x + y * y);
		if (d > radius) return 0.0f;
		else return 1 - d / radius;
	}
	virtual int getSupportRadius() {
		return floor(radius + 0.4999);
	}
};

class GaussianFilter :public Filter {
private:
	float sigma;
public:
	GaussianFilter(float sigma):sigma(sigma){}
	virtual float getWeight(float x, float y) {
		float d = sqrt(x*x + y * y);
		return exp(-(d*d) / (2 * sigma*sigma));
	}
	virtual int getSupportRadius() {
		return floor(2 * sigma + 0.4999);
	}
};

