#ifndef GREYBOXTSP_H
#define GREYBOXTSP_H
#include "vector"
#include "graph.h"
#include "tspPermutation.h"
#include <functional>

template<class T>
class GreyBoxEA
{
public:
	GreyBoxEA(
		std::vector<TSPpermutation>& population,
		unsigned int _maxNumberOfIterations,
		double _mutationProb,
		double _crossoverProb
	);

	std::vector<T>& population;
	unsigned int maxNumberOfIterations;
	unsigned int currentNumberOfIterations = 0;
	double mutationProb;
	double crossoverProb;

	bool iterate(const Graph& graph);
};
#endif
