#include "pch.h"
#include "tspPermutation.h"
#include <algorithm>
#include <numeric>
#include <random>



TSPpermutation::TSPpermutation(int size, const Graph& graph)
{
	// fill order with [size] numbers from 0 to size-1
	std::vector<int> order(size);
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

}




