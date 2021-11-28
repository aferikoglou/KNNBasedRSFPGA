#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <sys/time.h>
#include <math.h>

#include "sds_lib.h"
#include "calcDist.h"

#define k 10

using namespace std;

class perf_counter
{
public:
     uint64_t tot, cnt, calls, time_cnt;
     struct timeval T0,T1,res;
     struct timezone tz;
     perf_counter() : tot(0), cnt(0), calls(0), time_cnt(0) {};
     inline void reset()
     {
    	 tot = cnt = calls = 0;
     }
     inline void start()
     {
    	 cnt = sds_clock_counter();
    	 calls++;
     };
     inline void stop()
     {
    	 tot += (sds_clock_counter() - cnt);
     };
     inline uint64_t avg_cpu_cycles()
     {
    	 return ((tot+(calls>>1)) / calls);
     };
};

void parseDataset(float* data)
{
	/*****
	 * Parse dataset.csv file that for each selected movie contains the rating
	 * for all the selected users.
	 *****/

	string DATASET_PATH = "dataset.csv";
	ifstream inputFile(DATASET_PATH.c_str());

	int numOfLines = 0;
	int dataIndex = 0;
	string line;
	while (getline(inputFile, line))
	{
		istringstream iss(line);

		vector<float> linePartsVector;
		for (float i; iss >> i;) {
			linePartsVector.push_back(i);
			if (iss.peek() == ',')
				iss.ignore();
		}

		if (numOfLines == 0) {
			numOfLines++;
			continue;
		}

		for (long unsigned int i = 1; i < USERS_NUM + 1; i++) { // Instead of linePartsVector.size() we use MAX_FEATURES_NUM
			data[dataIndex] = linePartsVector[i];
			dataIndex++;
		}
		numOfLines++;

		if (numOfLines == MOVIES_NUM + 1)
			break;

	}
}

void parseNameIdMapping(map<int, string>& nameIdMapping)
{
	/*****
	 * Parse nameIdMapping.csv that specifies in which movie name each line
	 * corresponds to
	 *****/

	string NAME_ID_MAPPING_PATH = "nameIdMapping.csv";
	ifstream inputFile(NAME_ID_MAPPING_PATH.c_str());

	string line;
	while (getline(inputFile, line))
	{
		istringstream iss(line);

		vector<string> linePartsVector;
		while (iss.good()) {
			string substr;
			getline(iss, substr, ',');
			linePartsVector.push_back(substr);
		}

		int movieId = atoi(linePartsVector[0].c_str());
		string movieName = linePartsVector[1];
		for (long unsigned int i = 2; i < linePartsVector.size(); i++)
			movieName += " " + linePartsVector[i];

		nameIdMapping[movieId] = movieName;

	}
}

void copyArray(float* in, float* out, int arraySize)
{
	/*****
	 * Copy array in to array out
	 *****/

	for	(int i = 0; i < arraySize; i++)
		out[i] = in[i];
}

void calcDistances(float* data, float* dists)
{
	/*****
	 * Calculate the distance of movie with id MOVIE_ID from
	 * all the other movies
	 *****/

	for (int i = 0; i < MOVIES_NUM; i++) {
		float sum = 0.0, diff = 0.0;
		for (int j = 0; j < USERS_NUM; j++) {
			diff = data[MOVIE_ID * USERS_NUM + j] - data[i * USERS_NUM + j];
			sum += diff * diff;
		}
		dists[i] = sqrt(sum);
	}
}

bool containsMovieId(vector<int> moviesIdVector, int movieId)
{
	/*****
	 * Return 1 iff moviesIdVector contains movieId
	 *****/

	if (count(moviesIdVector.begin(), moviesIdVector.end(), movieId))
		return 1;
	return 0;
}

void getKNearestNeighbors(float* dists, vector<int>& moviesIdVector, vector<float>& moviesDists)
{
	/*****
	 * Get k closest movies
	 *****/

	float min;
	int index;
	for (int i = 0; i < k; i++) {
		min = FLT_MAX;
		index = -1;
		for (int j = 0; j < MOVIES_NUM; j++) {
			if (dists[j] < min && !containsMovieId(moviesIdVector, j)) {
				min = dists[j];
				index = j;
			}
		}

		moviesIdVector.push_back(index);
		moviesDists.push_back(min);
	}
}

void getRecommendations(float* dists, map<int, string>& nameIdMapping)
{
	/*****
	 * Get the k recommendations for each movie
	 *****/

	vector<int> moviesIdVector;
	vector<float> moviesDists;
	getKNearestNeighbors(dists, moviesIdVector, moviesDists);

	cout << "Recommendation system start to make inference" << endl;
	cout << "..." << endl;
	cout << "Recommendations for movie with id " << MOVIE_ID << ":" << endl;

	for (long unsigned int i = 0; i < moviesIdVector.size(); i++)
		cout << i << ". " << nameIdMapping[moviesIdVector[i]] << ", with distance of " << moviesDists[i] << endl;
}

int main(int argc, char **argv)
{
	float* data = (float *) malloc (MOVIES_NUM * USERS_NUM * sizeof(float));
	cout << "Started reading dataset..." << endl;
	parseDataset(data);
	cout << "Finished reading dataset..." << endl;

	map<int, string> nameIdMapping;
	cout << "Started reading name id mapping..." << endl;
	parseNameIdMapping(nameIdMapping);
	cout << "Finished reading name id mapping..." << endl;

	cout << "Input movie id = " << MOVIE_ID << endl;

	perf_counter HW_counter, SW_counter;

	// SOFTWARE EUCLIDEAN DISTANCE CALCULATION START
	float* dists = (float *) malloc (MOVIES_NUM * sizeof(float));

	cout << "Started distance calculations on software..." << endl;
	for(int i = 0; i < ITERATIONS; i++) {
		SW_counter.start();
		calcDistances(data, dists);
		SW_counter.stop();
	}
	cout << "Finished distance calculations on software..." << endl;
	dists[MOVIE_ID] = FLT_MAX; // We do not want to recommend the input movie
	// SOFTWARE EUCLIDEAN DISTANCE CALCULATION END

	// HARDWARE EUCLIDEAN DISTANCE CALCULATION START
	float* data_hw = (float *)sds_alloc(MOVIES_NUM * USERS_NUM * sizeof(float));
	float* dists_hw = (float *)sds_alloc(MOVIES_NUM * sizeof(float));

	if (!data_hw || !dists_hw) {
		if (data_hw)
			sds_free(data_hw);
		if (dists_hw)
			sds_free(dists_hw);
		return 1;
	}

	copyArray(data, data_hw, MOVIES_NUM * USERS_NUM);

	cout << "Started distance calculations on hardware..." << endl;
	for(int i = 0; i < ITERATIONS; i++) {
		HW_counter.start();
		calcDistancesHW(data_hw, dists_hw);
		HW_counter.stop();
	}
	cout << "Finished distance calculations on hardware..." << endl;
	dists_hw[MOVIE_ID] = FLT_MAX; // We do not want to recommend the input movie
	// HARDWARE EUCLIDEAN DISTANCE CALCULATION END

	cout << "" << endl;

	getRecommendations(dists_hw, nameIdMapping);

	cout << "" << endl;

	// SPEEDUP CALCULATION START
	uint64_t HW_cycles = HW_counter.avg_cpu_cycles();
	uint64_t SW_cycles = SW_counter.avg_cpu_cycles();

	double speedup = ((double) SW_cycles) / ((double) HW_cycles);

	cout << "Hardware cycles : " << HW_cycles << endl;
	cout << "Software cycles : " << SW_cycles << endl;
	cout << "Speedup         : " << speedup << endl;
	// SPEEDUP CALCULATION START

	// CORRECTNESS CALCULATION START
	double difference;
	int correct = 0;
	for(int i = 0; i < MOVIES_NUM; i++)
	{
		difference = dists[i] - dists_hw[i];
		if (abs(difference) < 0.001)
			correct++;
		else
			cout << "dists[" << i << "] = " << dists[i] << ", dists_hw[" << i << "] = " << dists_hw[i] << endl;
	}

	float score = (float)correct / (float)MOVIES_NUM;
	printf("Correct = %d, Score = %f \n", correct, score);
	// CORRECTNESS CALCULATION END

	free(data);
	free(dists);
	sds_free(data_hw);
	sds_free(dists_hw);

	return 0;
}
