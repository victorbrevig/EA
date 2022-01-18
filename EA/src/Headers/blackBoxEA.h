#ifndef BLACKBOXTSP_H
#define BLACKBOXTSP_H
#include "vector"
#include "graph.h"
#include "tspPermutation.h"
#include <functional>
#include "visualizer.h"
#include "permutationProblems.h"

template<class T>
class BlackBoxEA
{
public:

	struct Parameters {
		uint32_t population = 10;
		uint32_t iterations = 10000000;
		double mutationProb = 1.0;
		double crossoverProb = 0.3;
	};

	BlackBoxEA(
		std::vector<TSPpermutation>& population,
		unsigned int _maxNumberOfIterations,
		double _mutationProb,
		double _crossoverProb
	);

	std::vector<T>& population;
	unsigned int maxNumberOfIterations;
	unsigned int currentNumberOfIterations=0;
	double mutationProb;
	double crossoverProb;
	std::pair<uint32_t, uint32_t> selection(const Graph& graph);
	bool iterate(const Graph& graph);
	bool iterateGenerational(const Graph& graph);
	static PermutationProblems::Result Run(const Graph& graph, Parameters parameters, bool generational, Visualizer* visualizer = nullptr);
};
#endif