#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

#include <float.h> 

#define MOVIES_NUM 5392
#define USERS_NUM 28346

using namespace std;

void parseDataset(float* data)
{
	/*****
	 * Parse dataset.csv file that for each selected movie contains the rating
	 * for all the selected users.
	 *****/
	 
	string DATASET_PATH = "./dataset/dataset.csv";
	ifstream inputFile(DATASET_PATH);
	
	int numOfLines = 0;
	int dataIndex = 0;
	string line;
	while (getline(inputFile, line))
	{
		istringstream iss(line);

		// Split line by comma
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
		
		for (long unsigned int i = 1; i < linePartsVector.size(); i++) {
			data[dataIndex] = linePartsVector[i];
			dataIndex++;
		}
		numOfLines++;
	}
}


void parseNameIdMapping(map<int, string>& nameIdMapping)
{
	/*****
	 * Parse nameIdMapping.csv that specifies in which movie name each line
	 * corresponds to
	 *****/
	 
	string NAME_ID_MAPPING_PATH = "./dataset/nameIdMapping.csv";
	ifstream inputFile(NAME_ID_MAPPING_PATH);
	
	string line;
	while (getline(inputFile, line))
	{
		istringstream iss(line);

		// Split line by comma
		vector<string> linePartsVector;
		while (iss.good()) {
			string substr;
			getline(iss, substr, ',');
			linePartsVector.push_back(substr);
		}
				
		int movieId = stoi(linePartsVector[0]);	
		string movieName = linePartsVector[1];	
		for (long unsigned int i = 2; i < linePartsVector.size(); i++)
			movieName += " " + linePartsVector[i];
		
		nameIdMapping[movieId] = movieName;
	}	
}

int getMovieId(map<int, string>& nameIdMapping, string movieName)
{
	/*****
	 * Get the id of the movie that corresponds to the provided movieName
	 * Returns -1 if the movie is not found
	 *****/
	
	int movieId = -1;
	for (auto it = nameIdMapping.begin(); it != nameIdMapping.end(); ++it)
		if (it->second == movieName)
			movieId = it->first;
	return movieId;
}

float manhattanDistance(float* data, int movieId1, int movieId2)
{
        /*****
         * Get the manhattan distance of 2 movies
         *****/
        float sum = 0.0, diff = 0.0;
        for (int i = 0; i < USERS_NUM; i++) {
                diff = abs(data[movieId1 * USERS_NUM + i] - data[movieId2 * USERS_NUM + i]);
                sum += diff;
        }

        return sum;
}

float euclideanDistance(float* data, int movieId1, int movieId2)
{
	/*****
	 * Get the euclidean distance of 2 movies
	 *****/
	float sum = 0.0, diff = 0.0;
	for (int i = 0; i < USERS_NUM; i++) {
		diff = data[movieId1 * USERS_NUM + i] - data[movieId2 * USERS_NUM + i];
		sum += diff * diff; 
	}
	
	return sqrt(sum);
}

float minkowskiDistance(float* data, int movieId1, int movieId2, float p)
{
        /*****
         * Get the minkowski distance of order p of 2 movies
         *****/
        float sum = 0.0, diff = 0.0;
        for (int i = 0; i < USERS_NUM; i++) {
                diff = abs(data[movieId1 * USERS_NUM + i] - data[movieId2 * USERS_NUM + i]);
                sum += pow(diff, p);
        }

        return pow(sum, 1.0 / p);
}

float cosineDistance(float* data, int movieId1, int movieId2)
{
        /*****
         * Get the cosine distance of 2 movies
         *****/
        float sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
        for (int i = 0; i < USERS_NUM; i++) {
                sum1 += data[movieId1 * USERS_NUM + i] * data[movieId2 * USERS_NUM + i];
                sum2 += data[movieId1 * USERS_NUM + i] * data[movieId1 * USERS_NUM + i];
		sum3 += data[movieId2 * USERS_NUM + i] * data[movieId2 * USERS_NUM + i];
        }

	float cos = sum1 / (sqrt(sum2) * sqrt(sum3));

        return (1 - cos);
}

float hammingDistance(float* data, int movieId1, int movieId2)
{
        /*****
         * Get the hamming distance of 2 movies
         *****/
        float sum = 0.0;
        for (int i = 0; i < USERS_NUM; i++) {
                sum += (data[movieId1 * USERS_NUM + i] != data[movieId2 * USERS_NUM + i]) ? 1.0 : 0.0;
        }

        return sum;
}

void calcDistances(float* data, int movieId, int distanceMetric, float p, float* dists)
{
	/*****
	 * Calculate the distances of all the different movies
	 *****/
	 
	for (int i = 0; i < MOVIES_NUM; i++) {
		
		if (i != movieId) {

			// Calculate the distance between two movies
			if (distanceMetric == 1)
				dists[i] = manhattanDistance(data, i, movieId);
			else if (distanceMetric == 2)
				dists[i] = cosineDistance(data, i, movieId);
			else if (distanceMetric == 3)
				dists[i] = minkowskiDistance(data, i, movieId, p);
			else if (distanceMetric == 4)
				dists[i] = hammingDistance(data, i, movieId);
			else
				dists[i] = euclideanDistance(data, i, movieId);			
		} else
			dists[i] = FLT_MAX; // We do not want our system to propose our favorite movie

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

void getKNearestNeighbors(float* dists, int k, vector<int>& moviesIdVector, vector<float>& moviesDists)
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

void KNNMovieRecommendationSystem(string movieName, int k, int distanceMetric, float p, float* data, map<int, string> nameIdMapping)
{
	/*****
	 * Get the name of a movie in the dataset and recommend k movies
	 * using KNN
	 *****/
	
	cout << "Input movie = " << movieName << endl;
	int movieId = getMovieId(nameIdMapping, movieName);

	/*****
	cout << movieId << endl;
	*****/
	
	float* dists = (float *) malloc (MOVIES_NUM * sizeof(float));	
	calcDistances(data, movieId, distanceMetric, p, dists);
	
	/*****
	for (int i = 0; i < MOVIES_NUM; i++)
		cout << dists[i] << endl;
	*****/
	
	vector<int> moviesIdVector;
	vector<float> moviesDists;
	 
	getKNearestNeighbors(dists, k, moviesIdVector, moviesDists);
	
	cout << "Recommendation system start to make inference" << endl;
	cout << "..." << endl;
	cout << "Recommendations for " << movieName << ":" << endl;
	
	for (long unsigned int i = 0; i < moviesIdVector.size(); i++)
        	cout << i << ". " << nameIdMapping[moviesIdVector[i]] << ", with distance of " << moviesDists[i] << endl;
        	
        free(dists);
}


int main(int argc, char **argv)
{
	if (argc != 4 && argc != 5) {
		cout << "Invalid input !" << endl;
		cout << "./knn-recommendation.exe [movieName] [k] [distanceMetric]" << endl;
		cout << "The default distanceMetric is euclidean (0). You can also use manhattan (1), cosine (2), p-minkowski (3) and hamming (4) distance." << endl;
		cout << "In case of p-minkowski distance you can also specify the p parameter (default p value is 0.5)." << endl;

		return(1);
	}
	
	string movieName = argv[1];
	int k = stoi(argv[2]);
	int distanceMetric = stoi(argv[3]);

	float p = 0.5;
	if (argc == 5)
		p = stof(argv[4]);
	
	if (distanceMetric == 1)
		cout << "Distance metric: Manhattan" << endl;
	else if (distanceMetric == 2)
		cout << "Distance metric: Cosine" << endl;
	else if (distanceMetric == 3)
		cout << "Distance metric: Minkowski (p = " << p << ")" << endl;
	else if (distanceMetric == 4)
		cout << "Distance metric: Hamming" << endl;
	else
		cout << "Distance metric: Euclidean" << endl;
	
	float* data = (float *) malloc (MOVIES_NUM * USERS_NUM * sizeof(float));
	
	cout << "Started reading dataset..." << endl; 
	parseDataset(data);
	cout << "Finished reading dataset..." << endl;
	
	/*****
	for (int i = 0; i < USERS_NUM - 1; i++)
		cout << data[i] << ", ";
	cout << data[USERS_NUM - 1] << endl;
	*****/
	
	map<int, string> nameIdMapping;
	
	cout << "Started reading name id mapping..." << endl; 
	parseNameIdMapping(nameIdMapping);
	cout << "Finished reading name id mapping..." << endl; 

	/*****
	map<int, string>::iterator it;
	for (it = nameIdMapping.begin(); it != nameIdMapping.end(); it++)
		cout << "nameIdMapping[ " << it->first << "] = " << it->second << endl;
	*****/

	KNNMovieRecommendationSystem(movieName, k, distanceMetric, p, data, nameIdMapping);
	
	free(data);

	return 0;
}
