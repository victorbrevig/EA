#ifndef TSPPERMUTATION_H
#define TSPPERMUTATION_H
#include "vector"
#include "graph.h"

class TSPpermutation
{
public:
	TSPpermutation(unsigned int numberOfVertices);
	TSPpermutation(const std::vector<uint32_t>& _order);
	double fitness=0.0;
	std::vector<uint32_t> order;
	void mutate_2OPT();
	void updateFitness(const Graph& graph);
	static TSPpermutation orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm);

};
#endif