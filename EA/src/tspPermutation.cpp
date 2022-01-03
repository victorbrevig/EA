#include "pch.h"
#include "tspPermutation.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <stdlib.h>



TSPpermutation::TSPpermutation(const Graph& graph)
{
	// fill order with [size] numbers from 0 to size-1
	std::vector<int> order(graph.GetNumberOfVertices());
	std::iota(std::begin(order), std::end(order), 0);

	std::shuffle(order.begin(), order.end(), std::mt19937{ std::random_device{}() });
	updateFitness(graph);

}

void TSPpermutation::mutate_2OPT()
{
}

void TSPpermutation::updateFitness(const Graph& graph)
{
	fitness = graph.calculateDistByOrder(order);
}

void TSPpermutation::orderCrossover(const TSPpermutation& matePermutation)
{
	std::vector<uint32_t> child(order.size());

	// boolean vector to check if an element has been inserted in the child permutation
	std::vector<bool> bVec(order.size());
	std::fill(bVec.begin(), bVec.end(), false);

	

	// random index between 0 and permutation size - 1 (inclusive)
	unsigned int l = rand() % order.size();

	// random index between 0 and l - 1 (inclusive)
	unsigned int k = (l == 0) ? 0 : rand() % l;

	// values from index k to l (inclusive) are assigned to the same as the first parent
	for (unsigned int i = k; i <= l; i++) {
		child[i] = order[i];
		bVec[order[i]] = true;
	}

	// fill from second parent starting at l+1 
	unsigned int yIndex = l + 1;
	for (unsigned int i = l + 1; i < k; i = (i + 1) % order.size()) {
		if (!bVec[matePermutation.order[i]]) {
			child[i] = matePermutation.order[i];
			bVec[matePermutation.order[i]] = true;
			yIndex++;
		}
		else {

		}
	}

}




