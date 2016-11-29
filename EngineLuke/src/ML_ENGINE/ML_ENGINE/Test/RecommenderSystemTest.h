#pragma once
#include <ostream>

// Prints recommendation for users with given critics data.
void RecommenderSystemTest_Movies(
	const std::string& criticsFilename, // critics file for critics dataframe
	const std::string& usersFilename, // users file for users dataframe
	std::ostream* debug_out, // prints debug output to see process detail
	std::ostream* out); // stream to print table

void RecommenderSystemTest_Wines(
  const std::string& winefilename,
  std::ostream* debug_out,
  std::ostream& out);
