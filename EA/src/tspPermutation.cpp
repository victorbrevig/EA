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

	struct Edge_hash {
		inline std::size_t operator()(const Edge& v) const {
			return v.from > v.to ? (v.from * 31 + v.to) : (v.to * 31 + v.from);
		}
	};
	struct Edge_equals {
		bool operator()(const Edge& v1, const Edge& v2) const {
			return (v1.from == v2.from && v1.to == v2.to) || (v1.from == v2.to && v1.to == v2.from);
		}
	};
	struct EdgeOwner_hash {
		inline std::size_t operator()(const EdgeOwner& v) const {
			return v.from > v.to ? (v.from * 31 + v.to) : (v.to * 31 + v.from);
		}
	};
	struct EdgeOwner_equals {
		bool operator()(const EdgeOwner& v1, const EdgeOwner& v2) const {
			return (v1.from == v2.from && v1.to == v2.to) || (v1.from == v2.to && v1.to == v2.from);
		}
	};
	
	// REMOVE ALL COMMON EDGES
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> nonCommonEdges;
	std::vector<Edge> commonEdges;
	// Insert all edges from first parent in a set
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> firstParentEdges;
	uint32_t permSize = firstPerm.order.size();

	
	EdgeOwner edge = { firstPerm.order[0], firstPerm.order[1], true };
	firstParentEdges.emplace(edge);

	
	for (int i = 1; i <= permSize; i++) {
		EdgeOwner edge = { firstPerm.order[i - 1], firstPerm.order[i % permSize], true };
		firstParentEdges.emplace(edge);
	}
	
	
	// Loop through edges of second parent and check whether they (or the reverse) are contained in the set
	std::tuple<uint32_t, uint32_t, bool> edgeReverse;
	for (int i = 1; i <= permSize; i++) {
		EdgeOwner edge = { secondPerm.order[i - 1], secondPerm.order[i % permSize], false };
		std::unordered_set<EdgeOwner>::iterator it = firstParentEdges.find(edge);

		if (it != firstParentEdges.end()) {
			// remove edge if common
			firstParentEdges.erase(it);
			Edge e = { secondPerm.order[i - 1], secondPerm.order[i % permSize] };
			commonEdges.push_back(e);
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
		undirGraph.addEdge(e.from, e.to, e.isFirstParent);
	}


	// IDENTIFY CONNECTED COMPONENTS USING BFS
	std::unordered_set<uint32_t> remainingVertices(permSize);
	// fill remainingVertices with all vertices to begin with
	for (uint32_t i = 0; i < permSize; i++) {
		remainingVertices.insert(i);
	}

	std::unordered_set<Edge, Edge_hash, Edge_equals> childEdges;

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

		// These are sets because we dont want to insert duplicates (which rise in the adjacency list since it is an undirected graph)
		std::unordered_set<Edge, Edge_hash, Edge_equals> firstParentCompEdges;
		std::unordered_set<Edge, Edge_hash, Edge_equals> secondParentCompEdges;

		// loop over vertices
		for (const auto& v : connectedComponent) {
			// loop over edges associated with that vertex
			for (auto i = undirGraph.adjLists[v].begin(); i != undirGraph.adjLists[v].end(); ++i) {
				std::pair<uint32_t, bool> edgeInfo = *i;
				if (edgeInfo.second) {
					// add to sumFirstParent
					sumFirstParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					Edge edge = { v, edgeInfo.first };
					firstParentCompEdges.insert(edge);
				}
				else {
					// add to sumSecondParent
					sumSecondParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					Edge edge = { v, edgeInfo.first };
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













	// CONVERT TO NEW PERMUTATION
	struct AdjVertex
	{
		uint32_t a = INT32_MAX;
		uint32_t b = INT32_MAX;
	};

	std::vector<AdjVertex> adjVertices(childEdges.size());

	for (Edge edge : childEdges)
	{
		if (adjVertices[edge.from].a == INT32_MAX)
			adjVertices[edge.from].a = edge.to;
		else
			adjVertices[edge.from].b = edge.to;

		if (adjVertices[edge.to].a == INT32_MAX)
			adjVertices[edge.to].a = edge.from;
		else
			adjVertices[edge.to].b = edge.from;
	}

	std::vector<uint32_t> finalOrder;
	finalOrder.reserve(adjVertices.size());
	uint32_t visiting = 0;
	uint32_t prevVisiting = 0;
	finalOrder.emplace_back(visiting);
	while (finalOrder.size() < adjVertices.size())
	{
		AdjVertex v = adjVertices[visiting];
		if (v.a == prevVisiting) {
			prevVisiting = visiting;
			visiting = v.b;
		}
		else {
			prevVisiting = visiting;
			visiting = v.a;
		}
		finalOrder.emplace_back(visiting);
	}

	return TSPpermutation(finalOrder);
}
