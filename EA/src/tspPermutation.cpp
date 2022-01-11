#include "pch.h"
#include "tspPermutation.h"
#include "undirectedGraph.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <stdlib.h>
#include <unordered_set>
#include <tuple>
#include "lksearch.h"

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

void TSPpermutation::mutate_doubleBridge()
{
	// get four distinct edges randomly chosen and sort
	std::unordered_set<uint32_t> edgesSet;
	while (edgesSet.size() != 4) {
		edgesSet.insert(Utils::Random::GetRange(0, (uint32_t)order.size() - 1));
	}
	std::vector<uint32_t> edges;
	for (const auto& e : edgesSet) {
		edges.push_back(e);
	}
	sort(edges.begin(), edges.end());

	std::vector<uint32_t> newOrder(order.size());
	uint32_t indexCount = 0;
	// insert A2
	for (uint32_t i = edges[0]+1; i <= edges[1]; i++) {
		newOrder[indexCount] = order[i];
		indexCount++;
	}
	// insert A1
	for (uint32_t i = (edges[3]+1)%order.size(); i != edges[0]+1; i=(i+1)%order.size()) {
		newOrder[indexCount] = order[i];
		indexCount++;
	}
	// insert A4
	for (uint32_t i = edges[2] + 1; i <= edges[3]; i++) {
		newOrder[indexCount] = order[i];
		indexCount++;
	}
	// insert A3
	for (uint32_t i = edges[1] + 1; i <= edges[2]; i++) {
		newOrder[indexCount] = order[i];
		indexCount++;
	}
	order = newOrder;
}

void TSPpermutation::LinKernighan(const Graph& graph, Visualizer* visualizer)
{
	LKSearch lkSearch(graph, visualizer);
	order = lkSearch.LinKernighan(order);
	updateFitness(graph);
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




std::optional<std::pair<TSPpermutation, TSPpermutation>> TSPpermutation::GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph)
{

	// REMOVE ALL COMMON EDGES
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> nonCommonEdges;
	std::vector<Edge> commonEdges;
	
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
		}
		else {
			// insert if non common
		 	nonCommonEdges.insert(edge);
		}
	}
	// insert edges from first parent that was not removed in loop above
	nonCommonEdges.insert(firstParentEdges.begin(), firstParentEdges.end());

	// used to identify consecutive common edges
	std::vector<AdjVertex> commonEdgesOfVerts(firstPerm.order.size());
	for (Edge edge : commonEdges)
	{
		if (commonEdgesOfVerts[edge.from].a == INT32_MAX)
			commonEdgesOfVerts[edge.from].a = edge.to;
		else if (commonEdgesOfVerts[edge.from].b == INT32_MAX)
			commonEdgesOfVerts[edge.from].b = edge.to;
		else
		{
			ASSERT(FALSE /* This should not happen*/);
		}
		if (commonEdgesOfVerts[edge.to].a == INT32_MAX)
			commonEdgesOfVerts[edge.to].a = edge.from;
		else if (commonEdgesOfVerts[edge.to].b == INT32_MAX)
			commonEdgesOfVerts[edge.to].b = edge.from;
		else
		{
			ASSERT(FALSE /* This should not happen*/);
		}
	}


	
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

	// CREATE Gu - GRAPH WITH NO COMMON EDGES
	UndirectedGraph Gu(permSize);
	// CREATE G - FULL GRAPH WITH COMMON EDGES (to remove components from)
	UndirectedGraph G(permSize);

	for (const auto& e : nonCommonEdges) {
		if (e.isFirstParent) {
			Gu.addEdge(e.from, e.to, 0);
			G.addEdge(e.from, e.to, 0);
		}
		else if (!e.isFirstParent) {
			Gu.addEdge(e.from, e.to, 1);
			G.addEdge(e.from, e.to, 1);
		}
	}
	for (const auto& e : commonEdges) {
		G.addEdge(e.from, e.to, 2);
	}


	// IDENTIFY CONNECTED COMPONENTS USING BFS
	std::unordered_set<uint32_t> remainingVertices(permSize);
	std::unordered_set<uint32_t> remainingVerticesInG(permSize);
	// fill remainingVertices with all vertices to begin with
	for (uint32_t i = 0; i < permSize; i++) {
		remainingVertices.insert(i);
		remainingVerticesInG.insert(i);
	}

	std::vector<Edge> childEdges;

	uint32_t numberOfConnectedComponents = 0;
	std::vector<uint32_t> componentCutCounts;

	uint32_t numberOfComponentsRemoved = 0;

	// track largest component for second child
	uint32_t largestCompSizeSeen = 0;
	std::vector<Edge> largestCompSecondParentEdges;
	std::vector<Edge> largestCompFirstParentEdges;
	bool firstParentSelectedInLargestComp;
	bool largestCompJustSet = false;
	uint32_t largestCompStartIndex;
	uint32_t largestCompEndIndex;

	while (remainingVertices.size() > 0) {
		// take first vertex in remainingVertices as start vertex for BFS
		uint32_t startVertex = *begin(remainingVertices);
		std::vector<uint32_t> connectedComponent = Gu.BFS(startVertex);

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
			// if v has a common edge, check if the final endpoint (over consecutive common edges) is in the connected component as well
			AdjVertex current = commonEdgesOfVerts[v];
			uint32_t nextVert;
			if (current.a == INT32_MAX && current.b == INT32_MAX) {
				continue;
			}
			else if (current.a != INT32_MAX && current.b != INT32_MAX) {
				// At this point, we should not be looking at vertices with two common edges
				ASSERT(false);
			}
			else if (current.a != INT32_MAX) {
				nextVert = current.a;
			}
			else {
				nextVert = current.b;
			}

			// loop through common edges to find end of chain
			uint32_t cameFrom = v;
			current = commonEdgesOfVerts[nextVert];
			while (current.a != INT32_MAX && current.b != INT32_MAX) {
				// both edges of current are common
				if (current.a == cameFrom) {
					cameFrom = nextVert;
					nextVert = current.b;
				}
				else {
					cameFrom = nextVert;
					nextVert = current.a;
				}
				current = commonEdgesOfVerts[nextVert];
			}

			// now nextVert is end of chain. Check if its in the connected component
			std::unordered_set<uint32_t>::const_iterator inConnectedComponentIt = connectedComponentVerts.find(nextVert);
			if (inConnectedComponentIt == connectedComponentVerts.end()) {
				// if endPoint not in connectedComponentVerts, increment cutCount
				cutCount++;
			}
		}

		componentCutCounts.push_back(cutCount);

		if (cutCount != 2) {
			// do nothing if cut size != 2
			continue;
		}

		if (connectedComponent.size() > largestCompSizeSeen) {
			largestCompSizeSeen = (uint32_t) connectedComponent.size();
			largestCompJustSet = true;
		}

		// if cutCount == 2 remove all vertices and their edges from G
		for (const auto& v : connectedComponent) {
			std::unordered_set<uint32_t>::iterator it = remainingVerticesInG.find(v);
			remainingVerticesInG.erase(it);
			G.removeVertexAndEdges(v);
		}

		numberOfComponentsRemoved++;

		// if cut==2, find best path over vertices
		for (const auto& v : connectedComponent) {
			// loop over edges associated with that vertex
			for (auto i = Gu.adjLists[v].begin(); i != Gu.adjLists[v].end(); ++i) {
				std::pair<uint32_t, uint32_t> edgeInfo = *i;
				if (v > edgeInfo.first) {
					continue;
				}
				if (edgeInfo.second == 0) {
					// add to sumFirstParent
					sumFirstParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					firstParentCompEdges.emplace_back(v, edgeInfo.first);
				}
				else if (edgeInfo.second == 1) {
					// add to sumSecondParent
					sumSecondParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					secondParentCompEdges.emplace_back(v, edgeInfo.first);
				}
			}
		}

		if (largestCompJustSet) {
			largestCompFirstParentEdges = firstParentCompEdges;
			largestCompSecondParentEdges = secondParentCompEdges;
		}

		// pick parent path with smallest sum:
		ASSERT(firstParentCompEdges.size() == secondParentCompEdges.size());
		if (sumFirstParent <= sumSecondParent) {
			if (largestCompJustSet) {
				firstParentSelectedInLargestComp = true;
				largestCompStartIndex = (uint32_t) childEdges.size();
				largestCompEndIndex = largestCompStartIndex + (uint32_t) firstParentCompEdges.size() - 1;
			}
			childEdges.insert(childEdges.end(), firstParentCompEdges.begin(), firstParentCompEdges.end());
		}
		else {
			if (largestCompJustSet) {
				firstParentSelectedInLargestComp = false;
				largestCompStartIndex = (uint32_t) childEdges.size();
				largestCompEndIndex = largestCompStartIndex + (uint32_t) secondParentCompEdges.size() - 1;
			}
			childEdges.insert(childEdges.end(), secondParentCompEdges.begin(), secondParentCompEdges.end());
		}
		firstParentCompEdges.clear();
		secondParentCompEdges.clear();
		largestCompJustSet = false;
	}

	// AT THIS POINT ALL CONNECTED COMPONENTS WITH CUT SIZE 2 HAS BEEN REMOVED FROM G
	// AND THE BEST PATHS FOR THESE HAVE BEEN APPENDED TO CHILDEDGES

	//std::cout << "Number of components: (not necessarily cut 2)" << numberOfConnectedComponents << "\n";

	// IF NO COMPONENTS WERE REMOVED WE CANNOT PARTITION WITH CUT 2, SO GPX NOT APPLICABLE
	if (numberOfComponentsRemoved == 0) {
		return std::nullopt;
	}

	// GO THROUGH THE REST OF G AND RUN BFS. APPEND BEST PATHS
	// need to find remaining vertices to start from
	while (remainingVerticesInG.size() > 0) {
		// take first vertex in remainingVertices as start vertex for BFS
		uint32_t startVert = *begin(remainingVerticesInG);
		std::vector<uint32_t> connectedComp = G.BFS(startVert);

		// remove vertices in connectedComponent from remainingVertices
		for (const auto& v : connectedComp) {
			std::unordered_set<uint32_t>::iterator it = remainingVerticesInG.find(v);
			remainingVerticesInG.erase(it);
		}

		if (connectedComp.size() > largestCompSizeSeen) {
			largestCompSizeSeen = (uint32_t) connectedComp.size();
			largestCompJustSet = true;
		}

		// add best path
		double sumFirstParent = 0;
		double sumSecondParent = 0;
		std::vector<Edge> firstParentCompEdges;
		std::vector<Edge> secondParentCompEdges;

		for (const auto& v : connectedComp) {
			// loop over edges associated with that vertex
			for (auto i = G.adjLists[v].begin(); i != G.adjLists[v].end(); ++i) {
				std::pair<uint32_t, uint32_t> edgeInfo = *i;
				if (v > edgeInfo.first) {
					continue;
				}
				if (edgeInfo.second == 0) {
					// add to sumFirstParent
					sumFirstParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					firstParentCompEdges.emplace_back(v, edgeInfo.first);
				}
				else if (edgeInfo.second == 1) {
					// add to sumSecondParent
					sumSecondParent += graph.calculateDistBetweenTwoVertices(v, edgeInfo.first);
					secondParentCompEdges.emplace_back(v, edgeInfo.first);
				}
			}
		}

		if (largestCompJustSet) {
			largestCompFirstParentEdges = firstParentCompEdges;
			largestCompSecondParentEdges = secondParentCompEdges;
		}

		// pick parent path with smallest sum:
		ASSERT(firstParentCompEdges.size() == secondParentCompEdges.size());
		if (sumFirstParent <= sumSecondParent) {
			if (largestCompJustSet) {
				firstParentSelectedInLargestComp = true;
				largestCompStartIndex = (uint32_t) childEdges.size();
				largestCompEndIndex = largestCompStartIndex + (uint32_t) firstParentCompEdges.size() - 1;
			}
			childEdges.insert(childEdges.end(), firstParentCompEdges.begin(), firstParentCompEdges.end());
		}
		else {
			if (largestCompJustSet) {
				firstParentSelectedInLargestComp = false;
				largestCompStartIndex = (uint32_t) childEdges.size();
				largestCompEndIndex = largestCompStartIndex + (uint32_t) secondParentCompEdges.size() - 1;
			}
			childEdges.insert(childEdges.end(), secondParentCompEdges.begin(), secondParentCompEdges.end());
		}
		firstParentCompEdges.clear();
		secondParentCompEdges.clear();
		largestCompJustSet = false;
	}



	// UNION WITH COMMON EDGES, append instead
	childEdges.insert(childEdges.end(), commonEdges.begin(), commonEdges.end());


	// create second child - same as first child byt pick other parent path in largest component
	std::vector<Edge> secondChildEdges = childEdges;

	// replace edges in largest connected comp
	uint32_t indexCount = largestCompStartIndex;
	if (firstParentSelectedInLargestComp) {
		for (const auto& e : largestCompSecondParentEdges) {
			secondChildEdges[indexCount] = e;
			indexCount++;
		}
	}
	else {
		for (const auto& e : largestCompFirstParentEdges) {
			secondChildEdges[indexCount] = e;
			indexCount++;
		}
	}


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

	for (uint32_t i = 0; i < childEdges.size(); i++)
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
	ASSERT(secondChildEdges.size() == firstPerm.order.size());

 	return { std::make_pair(TSPpermutation(fromEdgesToPermutation(childEdges)), TSPpermutation(fromEdgesToPermutation(secondChildEdges)))};
}


std::vector<uint32_t> TSPpermutation::fromEdgesToPermutation(const std::vector<Edge>& childEdges)
{

	// CONVERT TO NEW PERMUTATION
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
	for (uint32_t i = 0; i < childEdges.size(); i++)
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
	return finalOrder;
}


