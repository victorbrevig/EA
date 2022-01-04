#ifndef BLACKBOXTSP_H
#define BLACKBOXTSP_H
#include "vector"
#include "graph.h"
#include "tspPermutation.h"

class BlackBoxTSP
{
public:
	BlackBoxTSP(
		unsigned int _populationSize, 
		unsigned int _maxNumberOfIterations,
		float _mutationProb,
		float _corssoverProb
	);
	std::vector<TSPpermutation> population;
	unsigned int maxNumberOfIterations;
	unsigned int currentNumberOfIterations=0;
	float mutationProb;
	float crossoverProb;

	void iterate(const Graph& graph);
	


	

	

};
#endif