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

	std::shuffle(order.begin(), order.end(), std::mt19937{ std::random_device{}() });
	updateFitness(graph);

}

TSPpermutation::TSPpermutation(const Graph& graph, const std::vector<uint32_t>& _order)
{
	order = _order;
	updateFitness(graph);
}


void TSPpermutation::mutate_2OPT()
{
}

void TSPpermutation::updateFitness(const Graph& graph)
{
	fitness = graph.calculateDistByOrder(order);
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





