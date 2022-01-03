#ifndef TSPPERMUTATION_H
#define TSPPERMUTATION_H
#include "vector"
#include "graph.h"

class TSPpermutation
{
public:
	TSPpermutation(const Graph& graph);
	double fitness;
	std::vector<uint32_t> order;
	void mutate_2OPT();
	void updateFitness(const Graph& graph);
	void orderCrossover(const TSPpermutation& matePermutation);

};
#endif