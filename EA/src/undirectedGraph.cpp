#include "pch.h"
#include "undirectedGraph.h"



UndirectedGraph::UndirectedGraph(uint32_t _numOfVertices)
  : adjLists(std::vector<std::list<std::pair<uint32_t, uint32_t>>>(_numOfVertices)), numOfVertices(_numOfVertices)
{
}

void UndirectedGraph::addEdge(uint32_t src, uint32_t dest, uint32_t parent)
{
  // parent: 0 = first parent, 1 = second parent, 2 = both parents
  std::pair<uint32_t, uint32_t> e1(dest, parent);
  std::pair<uint32_t, uint32_t> e2(src, parent);
	adjLists[src].push_back(e1);
	adjLists[dest].push_back(e2);
}

void UndirectedGraph::addEdgeUnique(uint32_t src, uint32_t dest, uint32_t parent)
{
  // parent: 0 = first parent, 1 = second parent, 2 = both parents
  std::pair<uint32_t, uint32_t> e1(dest, parent);
  std::pair<uint32_t, uint32_t> e2(src, parent);
  for (auto& v : adjLists[src])
  {
    if (v.first == dest)
      return;
  }
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
          addEdgeUnique(i, edge.first);
      }
    }
  }
}


void UndirectedGraph::removeVertexFromList(uint32_t v, uint32_t toRemove)
{
    std::list<std::pair<uint32_t, uint32_t>>::iterator iter = adjLists[v].begin();
    std::list<std::pair<uint32_t, uint32_t>>::iterator end = adjLists[v].end();

    while (iter != end)
    {
        std::pair<uint32_t, uint32_t> listItem = *iter;

        if (listItem.first == toRemove) {
            iter = adjLists[v].erase(iter);
        }
        else {
            ++iter;
        }
    }
    
}

void UndirectedGraph::removeVertexAndEdges(uint32_t v)
{
    std::list<std::pair<uint32_t, uint32_t>> lst = adjLists[v];
    for (const auto& p : lst) {
        removeVertexFromList(p.first, v);
    }
    std::list<std::pair<uint32_t, uint32_t>> emptyList;
    adjLists[v] = emptyList;
}

std::vector<uint32_t> UndirectedGraph::BFS(uint32_t startVertex)
{

    std::vector<uint32_t> res;

    std::vector<bool> visited(numOfVertices, false);

    std::list<int> queue;

    res.push_back(startVertex);
    visited[startVertex] = true;
    queue.push_back(startVertex);

    std::list<std::pair<uint32_t, uint32_t>>::iterator i;

    while (!queue.empty()) {
        int currVertex = queue.front();

        queue.pop_front();

        for (i = adjLists[currVertex].begin(); i != adjLists[currVertex].end(); ++i) {
            std::pair<uint32_t,uint32_t> adjVertex = *i;
            if (!visited[adjVertex.first]) {
                res.push_back(adjVertex.first);
                visited[adjVertex.first] = true;
                queue.push_back(adjVertex.first);
            }
        }
    }
    return res;
}
