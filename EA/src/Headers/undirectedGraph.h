#ifndef UNDIRECTEDGRAPH_H
#define UNDIRECTEDGRAPH_H
#include "utils.h"
#include <list>


class UndirectedGraph {
	uint32_t numOfVertices;
	bool* visited;

public:
	std::list<std::pair<uint32_t, bool>>* adjLists;
	UndirectedGraph(uint32_t _numOfVertices);
	void addEdge(uint32_t src, uint32_t dest, bool parent);
	std::vector<uint32_t> BFS(uint32_t startVertex);
};
#endif