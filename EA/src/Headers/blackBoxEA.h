#ifndef BLACKBOXTSP_H
#define BLACKBOXTSP_H
#include "vector"
#include "graph.h"
#include "tspPermutation.h"
#include <functional>

template<class T>
class BlackBoxEA
{
public:
	BlackBoxEA(
		std::vector<TSPpermutation>& population,
		unsigned int _maxNumberOfIterations,
		double _mutationProb,
		double _corssoverProb
	);

	std::vector<T>& population;
	unsigned int maxNumberOfIterations;
	unsigned int currentNumberOfIterations=0;
	double mutationProb;
	double crossoverProb;

	bool iterate(const Graph& graph);
};
#endif