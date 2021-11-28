#include <math.h>

#include "calcDist.h"

void calcDistancesHW(float* data_hw, float* dists_hw)
{
	float data_hw_tmp[MOVIES_NUM][USERS_NUM];
	float movie_tmp[USERS_NUM];
	float dists_hw_tmp[MOVIES_NUM];

LOAD_DATA_HW_TMP:
	for (int i = 0; i < MOVIES_NUM; i++) {
		for (int j = 0; j < USERS_NUM; j++) {
			data_hw_tmp[i][j] = data_hw[i * USERS_NUM + j];
		}
	}

LOAD_MOVIE_TMP:
	for (int i = 0; i < USERS_NUM; i++){
		movie_tmp[i] = data_hw_tmp[MOVIE_ID][i];
	}

COMPUTE_DISTS:
	for (int i = 0; i < MOVIES_NUM; i++) {
		float sum = 0.0, diff = 0.0;
		for (int j = 0; j < USERS_NUM; j++){
			diff = data_hw_tmp[i][j] - movie_tmp[j];
			sum += diff * diff;
		}
		dists_hw_tmp[i] = sqrt(sum);
	}

WRITE_DISTS:
	for (int i = 0; i < MOVIES_NUM; i++) {
		dists_hw[i] = dists_hw_tmp[i];
	}
}
