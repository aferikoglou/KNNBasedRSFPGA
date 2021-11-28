#define MOVIES_NUM 1024
#define USERS_NUM 32

#define ITERATIONS 1024

#define MOVIE_ID 0

#pragma SDS data copy(data_hw[0:32768], dists_hw[0:32])
#pragma SDS data access_pattern(data_hw:SEQUENTIAL, dists_hw:SEQUENTIAL)
void calcDistancesHW(float* data_hw, float* dists_hw);
