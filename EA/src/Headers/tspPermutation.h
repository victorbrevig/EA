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
	bool fittnessIsValid;
	std::vector<uint32_t> order;
	bool mutate_2OPT(const Graph* graph = nullptr);
	void updateFitness(const Graph& graph);
	static TSPpermutation orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm);

};
#endif