#include "pch.h"
#include "tspPermutation.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <stdlib.h>


TSPpermutation::TSPpermutation(const Graph& graph)
	: order(graph.GetNumberOfVertices())
{
	// fill order with [size] numbers from 0 to size-1

	std::iota(std::begin(order), std::end(order), 0);

	std::shuffle(order.begin(), order.end(), Utils::Random::engine);
	fittnessIsValid = false;
	updateFitness(graph);

}

TSPpermutation::TSPpermutation(const Graph& graph, const std::vector<uint32_t>& _order)
{
	order = _order;
	updateFitness(graph);
}


bool TSPpermutation::mutate_2OPT(const Graph* graph)
{
	uint32_t k = Utils::Random::GetRange(0, (uint32_t)order.size() - 2);
	uint32_t l = Utils::Random::GetRange(k + 1, (uint32_t)order.size() - 1);

	if (graph != nullptr)
	{
		//See if this will lead to an improvement
		double introducedCost = graph->GetEdge(order[k], order[l]) + graph->GetEdge(order[k + 1], order[(l + 1) % order.size()]);
		double removedCost = graph->GetEdge(order[k], order[k + 1]) + graph->GetEdge(order[l], order[(l + 1) % order.size()]);
		if (removedCost < introducedCost)
			return false;
	}

	std::reverse(order.begin() + k + 1, order.begin() + l + 1);

	fittnessIsValid = false;
	return true;
}

void TSPpermutation::updateFitness(const Graph& graph)
{
	fitness = graph.calculateDistByOrder(order);
	fittnessIsValid = true;
}

TSPpermutation TSPpermutation::orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph)
{
	int permSize = firstPerm.order.size();
	std::vector<uint32_t> childOrder(permSize);

	// boolean vector to check if an element has been inserted in the child permutation
	std::vector<bool> bVec(permSize);
	std::fill(bVec.begin(), bVec.end(), false);

	// random index between 0 and permutation size - 1 (inclusive)
	unsigned int l = rand() % permSize;

	// random index between 0 and l - 1 (inclusive)
	unsigned int k = (l == 0) ? 0 : rand() % l;

	// values from index k to l (inclusive) are assigned to the same as the first parent
	for (unsigned int i = k; i <= l; i++) {
		childOrder[i] = firstPerm.order[i];
		bVec[firstPerm.order[i]] = true;
	}

	// fill from second parent starting at l+1 
	unsigned int yIndex = (l + 1u) % permSize;
	for (unsigned int i = (l + 1u)%permSize; i != k; i = (i + 1u) % permSize) {
		while (bVec[secondPerm.order[yIndex]]) {
			yIndex = (yIndex + 1u) % permSize;
		}
		childOrder[i] = secondPerm.order[yIndex];
		bVec[secondPerm.order[yIndex]] = true;
		yIndex = (yIndex + 1u) % permSize;
	}

	return TSPpermutation(graph, childOrder);
}





