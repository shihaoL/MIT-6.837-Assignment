#pragma once
#include "vectors.h"
#include <vector>
#include <random>
class Sampler {
public:
	int n;
	Sampler(int n):n(n) {

	}
	virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler : public Sampler {
private:
	//std::vector<Vec2f> s;
public:
	RandomSampler(int n):Sampler(n) {
	}
	Vec2f getSamplePosition(int n)override {
		static default_random_engine e;
		static uniform_real_distribution<float> u(0, 1);
		float x = u(e);
		float y = u(e);
		return { x,y };
	}
};

class UniformSampler :public Sampler {
public:
	UniformSampler(int n) :Sampler(n) {}
	Vec2f getSamplePosition(int n)override {
		int num = sqrt(this->n);
		float d = 1.0f / (num + 1);
		int x = n / num + 1, y = n % num + 1;
		return { x*d,y*d };
	}
};

class JitteredSampler: public Sampler
{
private:
	std::vector<Vec2f> s;
public:
	JitteredSampler(int n):Sampler(n){
	}
	Vec2f getSamplePosition(int n)override {
		static default_random_engine e;
		int num = sqrt(this->n);
		float d = 1.0f / (num + 1);
		uniform_real_distribution<float> u(-d, d);
		int i = n / num + 1;
		int j = n % num + 1;
		return { i*d + u(e),j*d + u(e) };
	}
};