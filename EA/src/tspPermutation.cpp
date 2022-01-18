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

	fitnessIsValid = false;

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

	fitnessIsValid = false;
}

void TSPpermutation::LinKernighan(const Graph& graph, Visualizer* visualizer)
{
	LKSearch lkSearch(graph, visualizer);
	order = lkSearch.LinKernighan(order);
	updateFitness(graph);
}

void TSPpermutation::updateFitness(const Graph& graph) const
{
	fitness = graph.calculateDistByOrder(order);
	fitnessIsValid = true;
}

int TSPpermutation::GetFitness() const
{
	ASSERT(fitnessIsValid);
	return fitness;
}

int TSPpermutation::GetFitness(const Graph& graph) const
{
	if (!fitnessIsValid)
		updateFitness(graph);
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




std::optional<std::pair<TSPpermutation, TSPpermutation>> TSPpermutation::GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats)
{
	if (stats)
	{
		stats->choices = 0;
		stats->twoCostComponents = 0;
	}
	// REMOVE ALL COMMON EDGES
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> nonCommonEdges;
	std::vector<Edge> commonEdges;
	
	// Insert all edges from first parent in a set
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> firstParentEdges;
	uint32_t permSize = (uint32_t)firstPerm.order.size();


	// partition cut size counters
	std::unordered_map<uint32_t, uint32_t> cutCounters;


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

		cutCounters[cutCount]++;



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

	uint32_t moreChoices = 0;
	// GO THROUGH THE REST OF G AND RUN BFS. APPEND BEST PATHS
	// need to find remaining vertices to start from
	while (remainingVerticesInG.size() > 0) {
		// take first vertex in remainingVertices as start vertex for BFS
		uint32_t startVert = *begin(remainingVerticesInG);
		std::vector<uint32_t> connectedComp = G.BFS(startVert);

		moreChoices++;

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

	TSPpermutation child1(fromEdgesToPermutation(childEdges));
	child1.updateFitness(graph);
	TSPpermutation child2(fromEdgesToPermutation(secondChildEdges));
	child2.updateFitness(graph);

	if (stats)
	{
		stats->twoCostComponents = cutCounters[2];
		stats->choices = stats->twoCostComponents + moreChoices;
	}

 	return { std::make_pair(child1, child2)};
}

std::vector<TSPpermutation::PartitionComponent> TSPpermutation::FindPartitionComponents(UndirectedGraph& G, UndirectedGraph& Gu, uint32_t permSize, const std::unordered_map<uint32_t, std::vector<uint32_t>>& commonEdgesOfVerts)
{
	std::vector<PartitionComponent> partitionComponents;

	// IDENTIFY CONNECTED COMPONENTS USING BFS
	std::unordered_set<uint32_t> remainingVertices(permSize);
	std::unordered_set<uint32_t> remainingVerticesInG(permSize);
	// fill remainingVertices with all vertices to begin with
	for (uint32_t i = 0; i < permSize; i++) {
		remainingVertices.insert(i);
		remainingVerticesInG.insert(i);
	}

	uint32_t endPointVertInComp = 0;

	while (remainingVertices.size() > 0) {
		// take first vertex in remainingVertices as start vertex for BFS
		TSPpermutation::PartitionComponent connectedComponent;

		uint32_t startVertex = *begin(remainingVertices);
		connectedComponent.vertices = Gu.BFS(startVertex);

		// remove vertices in connectedComponent from remainingVertices
		for (const auto& v : connectedComponent.vertices) {
			std::unordered_set<uint32_t>::iterator it = remainingVertices.find(v);
			remainingVertices.erase(it);
		}

		if (connectedComponent.vertices.size() == 1) {
			// nothing to chose if the connected component is just one vertex
			continue;
		}

		// get set of vertices in connectedComponent for fast lookup
		std::unordered_set<uint32_t> connectedComponentVerts;
		for (const auto& v : connectedComponent.vertices) {
			connectedComponentVerts.insert(v);
		}

		uint32_t cutCount = 0;
		// check if partition has cut = 2 or more
		for (const auto& v : connectedComponent.vertices) {
			// if v has a common edge, check if the final endpoint (over consecutive common edges) is in the connected component as well
			auto it = commonEdgesOfVerts.find(v);
			ASSERT(it != commonEdgesOfVerts.end());
			const std::vector<uint32_t>& commonEdges = it->second;
			for (const uint32_t& otherVert : commonEdges)
			{
				if (connectedComponentVerts.find(otherVert) == connectedComponentVerts.end())
				{
					cutCount++;
					if (cutCount == 1)
						connectedComponent.vertexA = v;
					if (cutCount == 2)
						connectedComponent.vertexB = v;
				}
			}
		}

		connectedComponent.cutCost = cutCount;

		partitionComponents.emplace_back(std::move(connectedComponent));
	}


	return partitionComponents;
}


std::optional<std::pair<TSPpermutation, TSPpermutation>> TSPpermutation::PXChained(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats)
{
	if (stats)
	{
		stats->choices = 0;
		stats->twoCostComponents = 0;
	}
	// REMOVE ALL COMMON EDGES
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> nonCommonEdges;
	std::vector<Edge> commonEdges;

	// Insert all edges from first parent in a set
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> firstParentEdges;
	uint32_t permSize = (uint32_t)firstPerm.order.size();


	// partition cut size counters
	std::unordered_map<uint32_t, uint32_t> cutCounters;


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
	std::unordered_map<uint32_t, std::vector<uint32_t>> commonEdgesOfVerts;
	for (uint32_t v : firstPerm.order)
		commonEdgesOfVerts[v] = std::vector<uint32_t>();
	for (Edge edge : commonEdges)
	{
		commonEdgesOfVerts[edge.from].push_back(edge.to);
		commonEdgesOfVerts[edge.to].push_back(edge.from);
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

	// CREATE Gu - GRAPH WITH NO COMMON EDGES (to find connected component of cut size 2)
	UndirectedGraph Gu(permSize);
	// CREATE G - FULL GRAPH WITH COMMON EDGES (to search for components in)
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

	std::vector<Edge> childEdges;

	//Find partition components
	std::vector<TSPpermutation::PartitionComponent> partitionComponents = FindPartitionComponents(G, Gu, permSize, commonEdgesOfVerts);


	uint32_t startVertex = UINT32_MAX;
	uint32_t chosenComponentSize = 0;
	std::unordered_set<uint32_t> partitionEndpoints;
	for (auto& component : partitionComponents)
	{
		cutCounters[component.cutCost]++;
		if (component.cutCost == 2)
		{
			if (startVertex == UINT32_MAX || (chosenComponentSize < component.vertices.size()))
			{
				//Might as well chose the biggest 2 cost component to start with
				startVertex = component.vertexA;
				chosenComponentSize = (uint32_t)component.vertices.size();
			}

			partitionEndpoints.insert(component.vertexA);
			partitionEndpoints.insert(component.vertexB);
		}
	}

	// WE CANNOT PARTITION WITH CUT 2, SO GPX NOT APPLICABLE
	if (startVertex == UINT32_MAX) {
		return std::nullopt;
	}

	auto GetVertexToOrderPos = [](const std::vector<uint32_t>& orderPosToVertex) -> std::vector<uint32_t> {
		std::vector<uint32_t> ret(orderPosToVertex.size());
		for (uint32_t i = 0; i < orderPosToVertex.size(); i++)
			ret[orderPosToVertex[i]] = i;
		return ret;
	};

	std::vector<uint32_t> p1VertexToOrderPos = GetVertexToOrderPos(firstPerm.order);
	std::vector<uint32_t> p2VertexToOrderPos = GetVertexToOrderPos(secondPerm.order);

	auto prev = [](const std::vector<uint32_t>& orderPosToVertex, const std::vector<uint32_t>& vertexToOrderPos, uint32_t vertex) -> uint32_t {
		return orderPosToVertex[vertexToOrderPos[vertex] == 0 ? (orderPosToVertex.size() - 1) : (vertexToOrderPos[vertex] - 1)];
	};

	auto next = [](const std::vector<uint32_t>& orderPosToVertex, const std::vector<uint32_t>& vertexToOrderPos, uint32_t vertex) -> uint32_t {
		return orderPosToVertex[(vertexToOrderPos[vertex] + 1) % orderPosToVertex.size()];
	};

	bool p1TraverseForward = true;
	bool p2TraverseForward = true;

	auto Step1 = [&p1TraverseForward, &p1orderPosToVertex = firstPerm.order, &p1VertexToOrderPos,  &prev, &next](uint32_t vertex) {
		if (p1TraverseForward)
			return next(p1orderPosToVertex, p1VertexToOrderPos, vertex);
		return prev(p1orderPosToVertex, p1VertexToOrderPos, vertex);
	};

	auto Step2 = [&p2TraverseForward, &p2orderPosToVertex = secondPerm.order, &p2VertexToOrderPos, &prev, &next](uint32_t vertex) {
		if (p2TraverseForward)
			return next(p2orderPosToVertex, p2VertexToOrderPos, vertex);
		return prev(p2orderPosToVertex, p2VertexToOrderPos, vertex);
	};

	uint32_t point1 = startVertex;
	uint32_t point2 = startVertex;

	//startVertex always has a common edge, so we can find a common direction
	p1TraverseForward = true;
	p2TraverseForward = true;
	if (Step1(startVertex) != Step2(startVertex))
	{
		p1TraverseForward = false;
		p2TraverseForward = true;
		if (Step1(startVertex) != Step2(startVertex))
		{
			p1TraverseForward = true;
			p2TraverseForward = false;
			if (Step1(startVertex) != Step2(startVertex))
			{
				p1TraverseForward = false;
				p2TraverseForward = false;
				ASSERT(Step1(startVertex) == Step2(startVertex));
			}
		}
	}

	//Let's just reverse this position, so we start going through a partition component
	p1TraverseForward = !p1TraverseForward;
	p2TraverseForward = !p2TraverseForward;


	std::vector<uint32_t> childOrder;
	childOrder.emplace_back(startVertex);

	//Return chain size
	auto ApplyChain1 = [&Step1](std::vector<uint32_t>& order, uint32_t v, uint32_t endVertex) -> uint32_t {
		uint32_t size = 0;
		while (v != endVertex)
		{
			v = Step1(v);
			order.emplace_back(v);
			size++;
		}
		return size;
	};

	auto ApplyChain2 = [&Step2](std::vector<uint32_t>& order, uint32_t v, uint32_t endVertex) -> uint32_t {
		uint32_t size = 0;
		while (v != endVertex)
		{
			v = Step2(v);
			order.emplace_back(v);
			size++;
		}
		return size;
	};

	auto TourCost1 = [&Step1, &graph](uint32_t v, uint32_t endVertex) {
		double cost = 0.0;
		while (v != endVertex)
		{
			uint32_t next = Step1(v);
			cost += graph.GetEdge(v, next);
			v = next;
		}
		return cost;
	};

	auto TourCost2 = [&Step2, &graph](uint32_t v, uint32_t endVertex) {
		double cost = 0.0;
		while (v != endVertex)
		{
			uint32_t next = Step2(v);
			cost += graph.GetEdge(v, next);
			v = next;
		}
		return cost;
	};

	uint32_t longestChain = 0;
	uint32_t longestChainStart = 0;
	uint32_t longestChainEnd = 0;
	std::vector<uint32_t> longestChainOtherOrder;
	uint32_t choices = 0;
	//This while loop is the interesting part
	//Here we actually combine the two parents using the Apply chain function
	while (childOrder.size() < permSize)
	{
		//Find chain
		std::unordered_set<uint32_t> needsVisit;
		uint32_t startPoint1 = point1;
		uint32_t startPoint2 = point2;
		uint32_t count = 0;
		do
		{
			count++;
			point1 = Step1(point1);
			point2 = Step2(point2);
			if (!needsVisit.erase(point1)) {
				// if vertex not present, then we insert it.
				needsVisit.insert(point1);
			}
			if (!needsVisit.erase(point2)) {
				// if vertex not present, then we insert it.
				needsVisit.insert(point2);
			}
		} while (!(needsVisit.empty() && (point1 == point2)));

		if (count > 1)
			choices++;

		//Apply best chain
		if (TourCost1(startPoint1, point1) < TourCost2(startPoint2, point2))
		{
			uint32_t chainLength = ApplyChain1(childOrder, startPoint1, point1);
			if (chainLength > longestChain)
			{
				longestChainOtherOrder.clear();
				longestChain = chainLength;
				longestChainEnd = (uint32_t)childOrder.size();
				longestChainStart = longestChainEnd - chainLength;
				ApplyChain2(longestChainOtherOrder, startPoint2, point2);
			}
		}
		else
		{
			uint32_t chainLength = ApplyChain2(childOrder, startPoint2, point2);
			if (chainLength > longestChain)
			{
				longestChainOtherOrder.clear();
				longestChain = chainLength;
				longestChainEnd = (uint32_t)childOrder.size();
				longestChainStart = longestChainEnd - chainLength;
				ApplyChain1(longestChainOtherOrder, startPoint1, point1);
			}
		}
	}
	ASSERT(childOrder.size() == permSize);


	TSPpermutation child1(childOrder);
	child1.updateFitness(graph);

	//The other order for longest chain
	for (uint32_t i = 0; i < longestChain; i++)
	{
		childOrder[longestChainStart + i] = longestChainOtherOrder[i];
	}

	TSPpermutation child2(childOrder);
	child2.updateFitness(graph);

#ifdef DEBUG
	ASSERT(child1.GetFitness(graph) <= firstPerm.GetFitness(graph));
	ASSERT(child1.GetFitness(graph) <= secondPerm.GetFitness(graph));
#endif // DEBUG

	if (stats)
	{
		stats->twoCostComponents = cutCounters[2];
		stats->choices = choices;
	}


	return { std::make_pair(child1, child2) };
}





std::optional<std::pair<TSPpermutation, TSPpermutation>> TSPpermutation::GPXImproved(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats)
{
	if (stats)
	{
		stats->choices = 0;
		stats->twoCostComponents = 0;
	}

	// REMOVE ALL COMMON EDGES
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> nonCommonEdges;
	std::vector<Edge> commonEdges;

	// Insert all edges from first parent in a set
	std::unordered_set<EdgeOwner, EdgeOwner_hash, EdgeOwner_equals> firstParentEdges;
	uint32_t permSize = (uint32_t)firstPerm.order.size();


	// partition cut size counters
	std::unordered_map<uint32_t, uint32_t> cutCounters;


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
	std::unordered_map<uint32_t, std::vector<uint32_t>> commonEdgesOfVerts;
	for (uint32_t v : firstPerm.order)
		commonEdgesOfVerts[v] = std::vector<uint32_t>();
	for (Edge edge : commonEdges)
	{
		commonEdgesOfVerts[edge.from].push_back(edge.to);
		commonEdgesOfVerts[edge.to].push_back(edge.from);
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

	// CREATE Gu - GRAPH WITH NO COMMON EDGES (to find connected component of cut size 2)
	UndirectedGraph Gu(permSize);
	// CREATE G - FULL GRAPH WITH COMMON EDGES (to search for components in)
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

	std::vector<Edge> childEdges;

	//Find partition components
	std::vector<TSPpermutation::PartitionComponent> partitionComponents = FindPartitionComponents(G, Gu, permSize, commonEdgesOfVerts);


	uint32_t startVertex = UINT32_MAX;
	uint32_t chosenComponentSize = 0;
	std::unordered_set<uint32_t> partitionEndpoints;
	for (auto& component : partitionComponents)
	{
		cutCounters[component.cutCost]++;
		if (component.cutCost == 2)
		{
			if (startVertex == UINT32_MAX || (chosenComponentSize < component.vertices.size()))
			{
				//Might as well chose the biggest 2 cost component to start with
				startVertex = component.vertexA;
				chosenComponentSize = (uint32_t)component.vertices.size();
			}

			partitionEndpoints.insert(component.vertexA);
			partitionEndpoints.insert(component.vertexB);
		}
	}

	// WE CANNOT PARTITION WITH CUT 2, SO GPX NOT APPLICABLE
	if (startVertex == UINT32_MAX) {
		return std::nullopt;
	}

	auto GetVertexToOrderPos = [](const std::vector<uint32_t>& orderPosToVertex) -> std::vector<uint32_t> {
		std::vector<uint32_t> ret(orderPosToVertex.size());
		for (uint32_t i = 0; i < orderPosToVertex.size(); i++)
			ret[orderPosToVertex[i]] = i;
		return ret;
	};

	std::vector<uint32_t> p1VertexToOrderPos = GetVertexToOrderPos(firstPerm.order);
	std::vector<uint32_t> p2VertexToOrderPos = GetVertexToOrderPos(secondPerm.order);

	auto prev = [](const std::vector<uint32_t>& orderPosToVertex, const std::vector<uint32_t>& vertexToOrderPos, uint32_t vertex) -> uint32_t {
		return orderPosToVertex[vertexToOrderPos[vertex] == 0 ? (orderPosToVertex.size() - 1) : (vertexToOrderPos[vertex] - 1)];
	};

	auto next = [](const std::vector<uint32_t>& orderPosToVertex, const std::vector<uint32_t>& vertexToOrderPos, uint32_t vertex) -> uint32_t {
		return orderPosToVertex[(vertexToOrderPos[vertex] + 1) % orderPosToVertex.size()];
	};

	bool p1TraverseForward = true;
	bool p2TraverseForward = true;

	auto Step1 = [&p1TraverseForward, &p1orderPosToVertex = firstPerm.order, &p1VertexToOrderPos, &prev, &next](uint32_t vertex) {
		if (p1TraverseForward)
			return next(p1orderPosToVertex, p1VertexToOrderPos, vertex);
		return prev(p1orderPosToVertex, p1VertexToOrderPos, vertex);
	};

	auto Step2 = [&p2TraverseForward, &p2orderPosToVertex = secondPerm.order, &p2VertexToOrderPos, &prev, &next](uint32_t vertex) {
		if (p2TraverseForward)
			return next(p2orderPosToVertex, p2VertexToOrderPos, vertex);
		return prev(p2orderPosToVertex, p2VertexToOrderPos, vertex);
	};

	uint32_t point1 = startVertex;
	uint32_t point2 = startVertex;

	auto UpdateTraverseDirection = [&p1TraverseForward, &p2TraverseForward, &Step1, &Step2](uint32_t startVertex) {
		//startVertex always has a common edge, so we can find a common direction
		p1TraverseForward = true;
		p2TraverseForward = true;
		if (Step1(startVertex) != Step2(startVertex))
		{
			p1TraverseForward = false;
			p2TraverseForward = true;
			if (Step1(startVertex) != Step2(startVertex))
			{
				p1TraverseForward = true;
				p2TraverseForward = false;
				if (Step1(startVertex) != Step2(startVertex))
				{
					p1TraverseForward = false;
					p2TraverseForward = false;
					ASSERT(Step1(startVertex) == Step2(startVertex));
				}
			}
		}

		//Let's just reverse this position, so we start going through a partition component
		p1TraverseForward = !p1TraverseForward;
		p2TraverseForward = !p2TraverseForward;
		ASSERT(Step1(startVertex) != Step2(startVertex));
	};

	UpdateTraverseDirection(startVertex);



	std::vector<uint32_t> child1Order;
	child1Order.emplace_back(startVertex);
	std::vector<uint32_t> child2Order;
	child2Order.emplace_back(startVertex);

	//Return chain size
	auto ApplyChain1 = [&Step1](std::vector<uint32_t>& order, uint32_t v, uint32_t endVertex) -> uint32_t {
		uint32_t size = 0;
		while (v != endVertex)
		{
			v = Step1(v);
			order.emplace_back(v);
			size++;
		}
		return size;
	};

	auto ApplyChain2 = [&Step2](std::vector<uint32_t>& order, uint32_t v, uint32_t endVertex) -> uint32_t {
		uint32_t size = 0;
		while (v != endVertex)
		{
			v = Step2(v);
			order.emplace_back(v);
			size++;
		}
		return size;
	};

	auto TourCost1 = [&Step1, &graph](uint32_t v, uint32_t endVertex) {
		double cost = 0.0;
		while (v != endVertex)
		{
			uint32_t next = Step1(v);
			cost += graph.GetEdge(v, next);
			v = next;
		}
		return cost;
	};

	auto TourCost2 = [&Step2, &graph](uint32_t v, uint32_t endVertex) {
		double cost = 0.0;
		while (v != endVertex)
		{
			uint32_t next = Step2(v);
			cost += graph.GetEdge(v, next);
			v = next;
		}
		return cost;
	};

	
	auto FillChildOrder = [&permSize, &Step1, &Step2, &TourCost1, &TourCost2, &ApplyChain1, &ApplyChain2, &partitionEndpoints, &UpdateTraverseDirection](std::vector<uint32_t>& childOrder, bool preferParent1)
	{
		int count1 = 0;
		int count2 = 0;
		uint32_t choices = 0;
		ASSERT(childOrder.size() == 1);
		uint32_t point1 = childOrder[0];
		uint32_t point2 = childOrder[0];
		ASSERT(point1 == point2);
		while (childOrder.size() < permSize)
		{
			//Find chain
			std::unordered_set<uint32_t> needsVisit;
			uint32_t startPoint1 = point1;
			uint32_t startPoint2 = point2;
			bool haltPoint1 = false;
			bool haltPoint2 = false;
			uint32_t count = 0;
			do
			{
				count++;
				if (!haltPoint1)
				{
					point1 = Step1(point1);
					if (partitionEndpoints.find(point1) != partitionEndpoints.end())
						haltPoint1 = true;
				}

				if (!haltPoint2)
				{
					point2 = Step2(point2);
					if (partitionEndpoints.find(point2) != partitionEndpoints.end())
						haltPoint2 = true;
				}

				if (!needsVisit.erase(point1)) {
					// if vertex not present, then we insert it.
					needsVisit.insert(point1);
				}
				if (!needsVisit.erase(point2)) {
					// if vertex not present, then we insert it.
					needsVisit.insert(point2);
				}
			} while (!(needsVisit.empty() && (point1 == point2)) && !haltPoint1 && !haltPoint2);

			if (count > 1)
				choices++; //This happens if the points diverted and there is a choice

			if (!(needsVisit.empty() && (point1 == point2)))
			{
				//Could not find common sub-chain so take prefered tour
				if (preferParent1)
				{
					while (!haltPoint1)
					{
						point1 = Step1(point1);
						if (partitionEndpoints.find(point1) != partitionEndpoints.end())
							haltPoint1 = true;
					}
					ApplyChain1(childOrder, startPoint1, point1);
					point2 = point1;
					UpdateTraverseDirection(point1);
					count1++;
				}
				else
				{
					while (!haltPoint2)
					{
						point2 = Step2(point2);
						if (partitionEndpoints.find(point2) != partitionEndpoints.end())
							haltPoint2 = true;
					}
					ApplyChain2(childOrder, startPoint2, point2);
					point1 = point2;
					UpdateTraverseDirection(point1);
					count2++;
				}
			}
			//Apply best chain
			else if (TourCost1(startPoint1, point1) < TourCost2(startPoint2, point2))
			{
				ApplyChain1(childOrder, startPoint1, point1);
			}
			else
			{
				ApplyChain2(childOrder, startPoint2, point2);
			}
		}

		if (childOrder.size() == permSize + 1)
		{
			ASSERT(childOrder.front() == childOrder.back());
			childOrder.pop_back();
		}

		ASSERT(childOrder.size() == permSize);

		return choices;
	};
	
	uint32_t choices = FillChildOrder(child1Order, true);
	FillChildOrder(child2Order, false);

	TSPpermutation child1(child1Order);
	child1.updateFitness(graph);

	TSPpermutation child2(child2Order);
	child2.updateFitness(graph);

#ifdef DEBUG
	uint32_t bestFitness = std::min(child1.GetFitness(graph), child2.GetFitness(graph));
	ASSERT(bestFitness <= firstPerm.GetFitness(graph));
	ASSERT(bestFitness <= secondPerm.GetFitness(graph));
#endif // DEBUG

	if (stats)
	{
		stats->twoCostComponents = cutCounters[2];
		stats->choices = choices;
	}

	return { std::make_pair(child1, child2) };
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


