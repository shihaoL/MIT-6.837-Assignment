#pragma once
#include <vector>
#include "matrix.h"
#include "image.h"
class IFS {
private:
	std::vector<Matrix> matries;
	std::vector<float> probabilities;
	int rand_pick();
public:
	IFS(const std::vector<Matrix>& _matries, const std::vector<float>& _probabilites) :matries(_matries), probabilities(_probabilites) {};
	~IFS() = default;
	void render(Image* img, int n, int iter);
};