#ifndef UNDIRECTEDGRAPH_H
#define UNDIRECTEDGRAPH_H
#include "utils.h"
#include <list>


class UndirectedGraph {
	uint32_t numOfVertices;
	void removeVertexFromList(uint32_t v, uint32_t toRemove);

public:
	std::vector<std::list<std::pair<uint32_t, uint32_t>>> adjLists;
	UndirectedGraph(uint32_t _numOfVertices);
	void addEdge(uint32_t src, uint32_t dest, uint32_t parent);
	void removeVertexAndEdges(uint32_t v);
	std::vector<uint32_t> BFS(uint32_t startVertex);
};
#endif