#ifndef TSPPERMUTATION_H
#define TSPPERMUTATION_H
#include "vector"
#include "graph.h"
#include <optional>
#include "visualizer.h"
#include "undirectedGraph.h"
#include <unordered_map>

class TSPpermutation
{
	mutable int fitness = 0;
	mutable bool fitnessIsValid;

	struct AdjVertex
	{
		uint32_t a = INT32_MAX;
		uint32_t b = INT32_MAX;
	};
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

	struct PartitionComponent
	{
		std::vector<uint32_t> vertices;
		uint32_t cutCost;

		//Vertex A and B are start and end vertices in 2 cost components
		//In other cost component they are some endpoints but here they dont matter
		uint32_t vertexA;
		uint32_t vertexB;
	};


public:

	struct Stats
	{
		uint32_t twoCostComponents;
		uint32_t choices;
	};

	TSPpermutation();
	TSPpermutation(unsigned int numberOfVertices);
	TSPpermutation(const std::vector<uint32_t>& _order);
	int GetFitness() const;
	int GetFitness(const Graph& graph) const;
	std::vector<uint32_t> order;
	bool mutate_2OPT(const Graph& graph, bool acceptWorse = false);
	void mutate_doubleBridge();
	void updateFitness(const Graph& graph) const;
	void LinKernighan(const Graph& graph, Visualizer* visualizer = nullptr);
	static TSPpermutation orderCrossover(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm);
	static std::vector<PartitionComponent> FindPartitionComponents(UndirectedGraph& G, UndirectedGraph& Gu, uint32_t permSize, const std::unordered_map<uint32_t, std::vector<uint32_t>>& commonEdgesOfVerts);
	static std::optional<std::pair<TSPpermutation, TSPpermutation>> GPX(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats = nullptr);
	static std::optional<std::pair<TSPpermutation, TSPpermutation>> PXChained(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats = nullptr);
	static std::optional<std::pair<TSPpermutation, TSPpermutation>> GPXImproved(const TSPpermutation& firstPerm, const TSPpermutation& secondPerm, const Graph& graph, Stats* stats = nullptr);
	static std::vector<uint32_t> fromEdgesToPermutation(const std::vector<Edge>& childEdges);

};
#endif