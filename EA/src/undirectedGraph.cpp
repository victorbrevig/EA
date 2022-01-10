#include "pch.h"
#include "undirectedGraph.h"



UndirectedGraph::UndirectedGraph(uint32_t _numOfVertices)
  : adjLists(std::vector<std::list<std::pair<uint32_t, bool>>>(_numOfVertices)), numOfVertices(_numOfVertices)
{
}

void UndirectedGraph::addEdge(uint32_t src, uint32_t dest, bool parent)
{
  std::pair<uint32_t, bool> e1(dest, parent);
  std::pair<uint32_t, bool> e2(src, parent);
	adjLists[src].push_back(e1);
	adjLists[dest].push_back(e2);
}
void UndirectedGraph::ImportEdges(const UndirectedGraph& graph, const std::vector<bool>& mask)
{
  //Get vertices from graph but only between vertices where mask[v] = 1
  ASSERT(mask.size() == adjLists.size());
  ASSERT(mask.size() == graph.adjLists.size());

  for (uint32_t i = 0; i < mask.size(); i++)
  {
    if (mask[i])
    {
      auto& edges = graph.adjLists[i];
      for (auto& edge : edges)
      {
        if (mask[edge.first])
          addEdge(i, edge.first);
      }
    }
  }
}

std::vector<uint32_t> UndirectedGraph::BFS(uint32_t startVertex)
{

    std::vector<uint32_t> res;

    std::vector<bool> visited(numOfVertices, false);

    std::list<int> queue;

    res.push_back(startVertex);
    visited[startVertex] = true;
    queue.push_back(startVertex);

    std::list<std::pair<uint32_t, bool>>::iterator i;

    while (!queue.empty()) {
        int currVertex = queue.front();

        queue.pop_front();

        for (i = adjLists[currVertex].begin(); i != adjLists[currVertex].end(); ++i) {
            std::pair<uint32_t,bool> adjVertex = *i;
            if (!visited[adjVertex.first]) {
                res.push_back(adjVertex.first);
                visited[adjVertex.first] = true;
                queue.push_back(adjVertex.first);
            }
        }
    }
    return res;
}
