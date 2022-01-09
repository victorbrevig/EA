#include "pch.h"
#include "tspPermutation.h"
#include "undirectedGraph.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <stdlib.h>
#include <unordered_set>
#include <tuple>
#include <unordered_map>

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
	int permSize = (int)firstPerm.order.size();
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




std::optional<TSPpermutation> TSPpermutation::GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph)
{
	struct Edge {
		Edge(uint32_t f, uint32_t t)
			: from(f), to(t) {}
		uint32_t from;
		uint32_t to;
	};
	struct EdgeOwner {
		EdgeOwner(uint32_t f, uint32_t t, bool i)
			: from(f), to(t), isFirstParent(i) {}
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
	// used to look up common edge when checking cut sizes
	std::unordered_map<uint32_t, uint32_t> vertToACommonEdgeNeighbor;
	
	// Insert all edges from first parent in a set
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> firstParentEdges;
	uint32_t permSize = (uint32_t)firstPerm.order.size();


	for (uint32_t i = 1; i <= permSize; i++) {
		firstParentEdges.emplace(firstPerm.order[i - 1], firstPerm.order[i % permSize], true);
	}


	// Loop through edges of second parent and check whether they (or the reverse) are contained in the set
	for (uint32_t i = 1; i <= permSize; i++) {
		EdgeOwner edge = { secondPerm.order[i - 1], secondPerm.order[i % permSize], false };
		std::unordered_set<EdgeOwner>::iterator it = firstParentEdges.find(edge);

		if (it != firstParentEdges.end()) {
			// remove edge if common
			firstParentEdges.erase(it);
			commonEdges.emplace_back(secondPerm.order[i - 1], secondPerm.order[i % permSize]);
			vertToACommonEdgeNeighbor[secondPerm.order[i - 1]] = secondPerm.order[i % permSize];
			vertToACommonEdgeNeighbor[secondPerm.order[i % permSize]] = secondPerm.order[i - 1];
		}
		else {
			// insert if non common
		 	nonCommonEdges.insert(edge);
		}
	}
	// insert edges from first parent that was not removed in loop above
	nonCommonEdges.insert(firstParentEdges.begin(), firstParentEdges.end());
	
#ifdef DEBUG
	uint32_t countCommon = 0;
	for (uint32_t i = 0; i < permSize; i++)
	{
		uint32_t from1 = firstPerm.order[i];
		uint32_t to1 = firstPerm.order[(i + 1) % permSize];
		for (uint32_t j = 0; j < permSize; j++)
		{
			uint32_t from2 = secondPerm.order[j];
			uint32_t to2 = secondPerm.order[(j + 1) % permSize];
			if (from1 == from2 && to1 == to2)
				countCommon++;
			else if (from1 == to2 && from2 == to1)
				countCommon++;
		}
	}
	ASSERT(countCommon == commonEdges.size());
#endif

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

	std::vector<Edge> childEdges;

	uint32_t numberOfConnectedComponents = 0;

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
		// Only count component if size>1 (effecitively same as fusing common edges)
		numberOfConnectedComponents++;

		// compute total length of each parent paths in connected component
		// note this will be times 2 since it is an adjacency list for an undirected graph
		double sumFirstParent = 0;
		double sumSecondParent = 0;

		// These are sets because we dont want to insert duplicates (which rise in the adjacency list since it is an undirected graph)
		std::vector<Edge> firstParentCompEdges;
		std::vector<Edge> secondParentCompEdges;

		// get set of vertices in connectedComponent for fast lookup
		std::unordered_set<uint32_t> connectedComponentVerts;
		for (const auto& v : connectedComponent) {
			connectedComponentVerts.insert(v);
		}

		uint32_t cutCount = 0;
		// check if partition has cut = 2 or more
		for (const auto& v : connectedComponent) {
			std::unordered_map<uint32_t, uint32_t>::const_iterator inCommonEdgeIt = vertToACommonEdgeNeighbor.find(v);
			if (inCommonEdgeIt != vertToACommonEdgeNeighbor.end()) {
				// v is part of a common edge, check if the other end is in the component
				uint32_t endPoint = inCommonEdgeIt->second;
				std::unordered_set<uint32_t>::const_iterator inConnectedComponentIt = connectedComponentVerts.find(endPoint);
				if (inConnectedComponentIt == connectedComponentVerts.end()) {
					// if endPoint not in connectedComponentVerts, increment cutCount
					cutCount++;
				}

			}
		}


		// if cut==2, find best path over vertices
		for (const auto& v : connectedComponent) {
			// loop over edges associated with that vertex
			for (auto i = undirGraph.adjLists[v].begin(); i != undirGraph.adjLists[v].end(); ++i) {
				std::pair<uint32_t, bool> edgeInfo = *i;
				if (v > edgeInfo.first) {
					continue;
				}
				if (edgeInfo.second) {
					// add to sumFirstParent
					sumFirstParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					firstParentCompEdges.emplace_back(v, edgeInfo.first);
				}
				else {
					// add to sumSecondParent
					sumSecondParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					secondParentCompEdges.emplace_back(v, edgeInfo.first);
				}
			}
		}

		// append instead of loop vector to vector !
		// pick parent path with smallest sum:
		ASSERT(firstParentCompEdges.size() == secondParentCompEdges.size());
		if (sumFirstParent <= sumSecondParent) {
			childEdges.insert(childEdges.end(), firstParentCompEdges.begin(), firstParentCompEdges.end());
		}
		else {
			childEdges.insert(childEdges.end(), secondParentCompEdges.begin(), secondParentCompEdges.end());
		}
		firstParentCompEdges.clear();
		secondParentCompEdges.clear();
	}

	std::cout << "Number of components: " << numberOfConnectedComponents << "\n";

	if (numberOfConnectedComponents == 1) {
		// NOT POSSIBLE TO DO CUT OF VALUE 2 - RETURN NOTHING
		return std::nullopt;
	}

	// UNION WITH COMMON EDGES, append instead
	childEdges.insert(childEdges.end(), commonEdges.begin(), commonEdges.end());

#ifdef DEBUG

	for (Edge edge : childEdges)
	{
		bool found = false;
		for (uint32_t i = 0; i < permSize; i++)
		{
			if (firstPerm.order[i] == edge.from && firstPerm.order[(i + 1) % permSize] == edge.to)
				found = true;
			if (firstPerm.order[i] == edge.to && firstPerm.order[(i + 1) % permSize] == edge.from)
				found = true;
			if (secondPerm.order[i] == edge.from && secondPerm.order[(i + 1) % permSize] == edge.to)
				found = true;
			if (secondPerm.order[i] == edge.to && secondPerm.order[(i + 1) % permSize] == edge.from)
				found = true;
		}
		ASSERT(found);
	}

	for (uint32_t i = 0; i < permSize; i++)
	{
		uint32_t count = 0;
		for (Edge edge : childEdges)
		{
			if (edge.from == i)
				count++;
			else if (edge.to == i)
				count++;
		}
		ASSERT(count == 2);
	}
#endif

	ASSERT(childEdges.size() == firstPerm.order.size());

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
		else if (adjVertices[edge.from].b == INT32_MAX)
			adjVertices[edge.from].b = edge.to;
		else
		{
			ASSERT(FALSE /* This should not happen*/);
		}
		if (adjVertices[edge.to].a == INT32_MAX)
			adjVertices[edge.to].a = edge.from;
		else if (adjVertices[edge.to].b == INT32_MAX)
			adjVertices[edge.to].b = edge.from;
		else
		{
			ASSERT(FALSE /* This should not happen*/);
		}
	}

#ifdef DEBUG
	for (uint32_t i = 0; i < permSize; i++)
	{
		uint32_t count = 0;
		for (AdjVertex adjVertex : adjVertices)
		{
			if (adjVertex.a == i)
				count++;
			else if (adjVertex.b == i)
				count++;
		}
		ASSERT(count == 2);
	}
#endif

	std::vector<uint32_t> finalOrder;
	finalOrder.reserve(adjVertices.size());
	uint32_t visiting = 0;
	uint32_t prevVisiting = adjVertices[visiting].b;
	finalOrder.emplace_back(visiting);
	while (finalOrder.size() < adjVertices.size())
	{
		AdjVertex& v = adjVertices[visiting];
		if (v.a == prevVisiting) {
			prevVisiting = visiting;
			visiting = v.b;
		}
		else if (v.b == prevVisiting) {
			prevVisiting = visiting;
			visiting = v.a;
		}
		else
		{
			ASSERT(FALSE /*This should not happen*/);
			break;
		}
		finalOrder.emplace_back(visiting);
	}

 	return { TSPpermutation(finalOrder) };
}
