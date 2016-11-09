#include "RecommenderSystemTest.h"
#include <fstream>

int main()
{
	// debug output stream that will be printed detailed process of 
	// recommendation system
	std::ofstream debug_out("Out/movieusers_debug.txt", std::ios::out);

	// final table output stream
	std::ofstream out("Out/movieusers_out.csv", std::ios::out);

	// test recommender system using movie data.
	RecommenderSystemTest_Movies(
		"Data/movie data.csv",
		"Data/movie data_test.csv",
		&debug_out, 
		&out);

	return 0;
}
