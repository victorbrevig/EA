#include "pch.h"
#include "tspPermutation.h"
#include "undirectedGraph.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <stdlib.h>
#include <unordered_set>
#include <tuple>

TSPpermutation::TSPpermutation()
{
	fitnessIsValid = false;
}

TSPpermutation::TSPpermutation(unsigned int numberOfVertices)
	: order(numberOfVertices)
{
	// fill order with [size] numbers from 0 to size-1
	std::iota(std::begin(order), std::end(order), 0);

	std::shuffle(order.begin(), order.end(), Utils::Random::engine);

	fitnessIsValid = false;
}

TSPpermutation::TSPpermutation(const std::vector<uint32_t>& _order)
{
	order = _order;
	fitnessIsValid = false;
}


bool TSPpermutation::mutate_2OPT(const Graph& graph, bool acceptWorse)
{
	uint32_t k = Utils::Random::GetRange(0, (uint32_t)order.size() - 2);
	uint32_t l = Utils::Random::GetRange(k + 1, (uint32_t)order.size() - 1);

	//See if this will lead to an improvement
	if (!acceptWorse)
	{
		double introducedCost = graph.GetEdge(order[k], order[l]) + graph.GetEdge(order[k + 1], order[(l + 1) % order.size()]);
		double removedCost = graph.GetEdge(order[k], order[k + 1]) + graph.GetEdge(order[l], order[(l + 1) % order.size()]);
		if (removedCost < introducedCost)
			return false;
	}

	std::reverse(order.begin() + k + 1, order.begin() + l + 1);

	updateFitness(graph);

	return true;
}

void TSPpermutation::updateFitness(const Graph& graph)
{
	fitness = graph.calculateDistByOrder(order);
	fitnessIsValid = true;
}

double TSPpermutation::GetFitness() const
{
	return fitness;
}

TSPpermutation TSPpermutation::orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm)
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

	return TSPpermutation(childOrder);
}




TSPpermutation TSPpermutation::GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph)
{

	struct Edge {
		uint32_t from;
		uint32_t to;
	};
	struct EdgeOwner {
		uint32_t from;
		uint32_t to;
		bool isFirstParent;
	};


	struct triplet_hash {
		inline std::size_t operator()(const std::tuple<uint32_t, uint32_t, bool>& v) const {
			return std::get<0>(v) * 31 + std::get<1>(v);
		}
	};
	struct triplet_equals {
		bool operator()(const std::tuple<uint32_t, uint32_t, bool>& v1, const std::tuple<uint32_t, uint32_t, bool>& v2) const {
			return std::get<0>(v1) == std::get<0>(v2) && std::get<1>(v1) == std::get<1>(v2);
		}
	};
	struct pair_hash {
		inline std::size_t operator()(const std::pair<uint32_t, uint32_t>& v) const {
			return v.first > v.second ? (v.first * 31 + v.second) : (v.second * 31 + v.first);
		}
	};
	struct pair_equals {
		bool operator()(const std::pair<uint32_t, uint32_t>& v1, const std::pair<uint32_t, uint32_t>& v2) const {
			return (v1.first == v2.first && v1.second == v2.second) || (v1.first == v2.second && v1.second == v2.first);;
		}
	};
	


	// REMOVE ALL COMMON EDGES
	std::unordered_set<std::tuple<uint32_t, uint32_t, bool>, triplet_hash, triplet_equals> nonCommonEdges;
	std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash> commonEdges;
	// Insert all edges from first parent in a set
	
	std::unordered_set<std::tuple<uint32_t, uint32_t, bool>, triplet_hash, triplet_equals> firstParentEdges;
	uint32_t permSize = firstPerm.order.size();

	
	std::tuple<uint32_t, uint32_t, bool> edge;
	firstParentEdges.emplace(firstPerm.order[0], firstPerm.order[1], true);

	
	for (int i = 1; i <= permSize; i++) {
		firstParentEdges.emplace(firstPerm.order[i - 1], firstPerm.order[i % permSize], true);
	}
	
	
	
	// Loop through edges of second parent and check whether they (or the reverse) are contained in the set
	std::tuple<uint32_t, uint32_t, bool> edgeReverse;
	for (int i = 1; i <= permSize; i++) {
		edge = std::make_tuple(secondPerm.order[i - 1], secondPerm.order[i%permSize], false);
		edgeReverse = std::make_tuple(secondPerm.order[i%permSize], secondPerm.order[i-1], false);
		std::unordered_set<std::tuple<uint32_t, uint32_t, bool>>::iterator it = firstParentEdges.find(edge);
		std::unordered_set<std::tuple<uint32_t, uint32_t, bool>>::iterator itR = firstParentEdges.find(edgeReverse);

		if (it != firstParentEdges.end()) {
			// remove edge if common
			firstParentEdges.erase(it);
			std::pair<uint32_t, uint32_t> commonEdge(secondPerm.order[i - 1], secondPerm.order[i % permSize]);
			commonEdges.insert(commonEdge);
		}
		else if (itR != firstParentEdges.end()) {
			// remove edge if common
			firstParentEdges.erase(itR);
			std::pair<uint32_t, uint32_t> commonEdge(secondPerm.order[i % permSize], secondPerm.order[i - 1]);
			commonEdges.insert(commonEdge);
		}
		else {
			// insert if non common
			nonCommonEdges.insert(edge);
		}
	}
	// insert edges from first parent that was not removed in loop above
	nonCommonEdges.insert(firstParentEdges.begin(), firstParentEdges.end());
	

	// CREATE GRAPH (WITH NON COMMON EDGES)
	UndirectedGraph undirGraph(permSize);

	for (const auto& e : nonCommonEdges) {
		undirGraph.addEdge(std::get<0>(e), std::get<1>(e), std::get<2>(e));
	}


	// IDENTIFY CONNECTED COMPONENTS USING BFS
	std::unordered_set<uint32_t> remainingVertices(permSize);
	// fill remainingVertices with all vertices to begin with
	for (uint32_t i = 0; i < permSize; i++) {
		remainingVertices.insert(i);
	}

	std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash, pair_equals> childEdges;

	while (remainingVertices.size() > 0) {
		// take first vertex in remainingVertices as start vertex for BFS
		uint32_t startVertex = *begin(remainingVertices);
		std::vector<uint32_t> connectedComponent = undirGraph.BFS(startVertex);

		// remove vertices in connectedComponent from remainingVertices
		for (const auto& v : connectedComponent) {
			std::unordered_set<uint32_t>::iterator it = remainingVertices.find(v);
			remainingVertices.erase(it);
		}

		if (connectedComponent.size() == 1) {
			// nothing to chose if the connected component is just one vertex
			continue;
		}
		
		// compute total length of each parent paths in connected component
		// note this will be times 2 since it is an adjacency list for an undirected graph
		double sumFirstParent = 0;
		double sumSecondParent = 0;

		std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash, pair_equals> firstParentCompEdges;
		std::unordered_set<std::pair<uint32_t, uint32_t>, pair_hash, pair_equals> secondParentCompEdges;

		// loop over vertices
		for (const auto& v : connectedComponent) {
			// loop over edges associated with that vertex
			for (auto i = undirGraph.adjLists[v].begin(); i != undirGraph.adjLists[v].end(); ++i) {
				std::pair<uint32_t, bool> edgeInfo = *i;
				if (edgeInfo.second) {
					// add to sumFirstParent
					sumFirstParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					std::pair<uint32_t, uint32_t> edge(v, edgeInfo.first);
					firstParentCompEdges.insert(edge);
				}
				else {
					// add to sumSecondParent
					sumSecondParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					std::pair<uint32_t, uint32_t> edge(v, edgeInfo.first);
					secondParentCompEdges.insert(edge);
				}
			}
		}

		// append instead of loop vector to vector !
		// pick parent path with smallest sum:
		if (sumFirstParent <= sumSecondParent) {
			for (const auto& e : firstParentCompEdges) {
				childEdges.insert(e);
			}
		}
		else {
			for (const auto& e : secondParentCompEdges) {
				childEdges.insert(e);
			}
		}



		firstParentCompEdges.clear();
		secondParentCompEdges.clear();
		
	}

	// UNION WITH COMMON EDGES, append instead
	for (const auto& e : commonEdges) {
		childEdges.insert(e);
	}

	// CONVERT TO NEW PATH
	// vector of all vertices, every vertex tells the next one (constrct from edges)

	std::vector<Edge> edges = {
		{0, 1},
		{1, 2},
		{3, 2},
		{4, 0},
		{4, 3},
	};

	struct AdjVertex
	{
		uint32_t a = INT32_MAX;
		uint32_t b = INT32_MAX;
	};

	std::vector<AdjVertex> adjVertices(edges.size() - 1);

	for (Edge edge : edges)
	{
		AdjVertex v1 = adjVertices[edge.from];
		if (v1.a == INT32_MAX)
			v1.a = edge.to;
		else
			v1.b = edge.to;

		AdjVertex v2 = adjVertices[edge.to];
		if (v2.a == INT32_MAX)
			v2.a = edge.from;
		else
			v2.b = edge.from;
	}

	std::vector<uint32_t> finalOrder;
	finalOrder.reserve(adjVertices.size());
	uint32_t visiting = 0;
	finalOrder.emplace_back(visiting);
	while (finalOrder.size() < adjVertices.size())
	{
		AdjVertex v = adjVertices[visiting];
		if (v.a == visiting)
			visiting = v.b;
		else
			visiting = v.a;
		finalOrder.emplace_back(visiting);
	}

	return TSPpermutation(4);
}







