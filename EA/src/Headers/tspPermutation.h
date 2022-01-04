#ifndef TSPPERMUTATION_H
#define TSPPERMUTATION_H
#include "vector"
#include "graph.h"

class TSPpermutation
{
public:
	TSPpermutation(const Graph& graph);
	TSPpermutation(const Graph& graph, const std::vector<uint32_t>& _order);
	double fitness;
	bool fittnessIsValid;
	std::vector<uint32_t> order;
	bool mutate_2OPT(const Graph* graph = nullptr);
	void updateFitness(const Graph& graph);
	static TSPpermutation orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph);

};
#endif