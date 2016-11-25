#include "RecommenderSystemTest.h"
#include <iostream>
#include <fstream>
#include "../ML/Dataframe/Dataframe.h"
#include "../ML/Recommender/RecommenderSystem.h"

// prints table to out stream in csv format
void PrintTable(std::ostream& out,
	Dataframe& dataframe,
	const std::vector<std::vector<double>>& table)
{
	// prints header
	for (size_t i = 1; i < dataframe.GetAttributeCount(); ++i)
	{
		out << ",";
		out << dataframe.GetAttributeName(i);
	}
	out << std::endl;

	// prints each user scores with their names
	for (size_t i = 0; i < dataframe.GetInstanceCount(); ++i)
	{
		// prints name
		const Instance& instance = dataframe.GetInstance(i);
		out << instance.GetAttribute(0).AsString();

		// prints product scores
		for (double col : table[i])
			out << "," << col;

		out << std::endl;
	}
}

void RecommenderSystemTest_Movies(
	const std::string& criticsFilename,
	const std::string& usersFilename,
	std::ostream* debug_out,
	std::ostream* out)
{
	// build critics dataframe
	Dataframe critics;
	if (!critics.BuildFromCsv(criticsFilename, true))
	{
		if(debug_out)
			*debug_out << "moviedata.csv doesn't exist!" << std::endl;
		return;
	}

	// build users dataframe
	Dataframe users;
	if (!users.BuildFromCsv(usersFilename, true))
	{
		if (debug_out)
			*debug_out << "movieata_test.csv doesn't exist!" << std::endl;
		return;
	}

	// set debug output stream for detailed view
	RecommenderSystem recommender(critics);
	recommender.SetDebugOutput(debug_out);

	// run recommender system for the given users and prints the result
	std::vector<std::vector<double>>&& table = recommender.RecommendFor(users);
	if(out)
		PrintTable(*out, users, table);
}
