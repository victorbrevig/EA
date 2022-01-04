#ifndef TSPPERMUTATION_H
#define TSPPERMUTATION_H
#include "vector"
#include "graph.h"

class TSPpermutation
{
	double fitness = 0.0;
	bool fitnessIsValid;

public:
	TSPpermutation();
	TSPpermutation(unsigned int numberOfVertices);
	TSPpermutation(const std::vector<uint32_t>& _order);
	double GetFitness() const;
	std::vector<uint32_t> order;
	bool mutate_2OPT(const Graph& graph, bool acceptWorse = false);
	void updateFitness(const Graph& graph);
	static TSPpermutation orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm);
	static TSPpermutation GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm);

};
#endif