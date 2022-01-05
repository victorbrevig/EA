#include "pch.h"
#include "lksearch.h"
#include <array>
#include <set>

bool IsWorse(double improvement)
{
	return improvement < -0.001; //Fix imprecision
}


bool IsImprovement(double improvement)
{
	return improvement > 0.001; //Fix imprecision
}

LKSearch::LKSearch(const Graph& graph, Visualizer* visualizer)
  : m_Graph(graph), m_Visualizer(visualizer)
{
}

void LKSearch::UpdateOrderFromVertex()
{
	if (m_CurrentOrderFromVertex.size() == 0)
		m_CurrentOrderFromVertex.resize(m_CurrentOrder.size());
	for (uint32_t i = 0; i < m_CurrentOrder.size(); i++)
		m_CurrentOrderFromVertex[m_CurrentOrder[i]] = i;
}

std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> LKSearch::GetLKOrdering(double delta)
{
	std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> lkOrdering;

	uint32_t nextBase = next(m_Base);

	const auto& neighborsBase = m_Graph.GetNearNeighborsOf(m_Base);
	const auto& neighborsBaseNext = m_Graph.GetNearNeighborsOf(nextBase);

	double baseToNextBase = m_Graph.GetEdge(m_Base, nextBase);
	for (const Graph::VertexDist& a : neighborsBaseNext)
	{
		if (prev(a.vertex) == nextBase)
			continue;
		if (IsImprovement(delta + baseToNextBase - a.distance))
		{
			//It is promising so maximize c(prev(a),a) - c(next(base), a), where a is candidate
			double reward = m_Graph.GetEdge(prev(a.vertex), a.vertex) - a.distance;
			lkOrdering.emplace(a.vertex, reward, false);
		}
	}

	//Mak-morton moves
	for (const Graph::VertexDist& a : neighborsBase)
	{
		if (next(a.vertex) == m_Base)
			continue;
		if (IsImprovement(delta + baseToNextBase - a.distance))
		{
			//It is promising so maximize c(prev(a),a) - c(next(base), a), where a is candidate
			double reward = m_Graph.GetEdge(a.vertex, next(a.vertex)) - a.distance;
			lkOrdering.emplace(a.vertex, reward, true);
		}
	}

	return lkOrdering;
}

uint32_t LKSearch::Breadth(uint32_t level)
{
	//return level > 2 ? 1 : 5;
	return level > 10 ? 0 : level > 2 ? 1 : 5;
}

double LKSearch::Step(uint32_t level, double delta)
{
 	uint32_t i = 1;
	uint32_t breadth = Breadth(level);
	if (breadth == 0)
		return 0.0;

	auto lkOrdering = GetLKOrdering(delta);
	while (!lkOrdering.empty() && i <= breadth)
	{
		LKSearch::PromissingVertex a = lkOrdering.top();
		lkOrdering.pop();
		double improvement = 0.0;
		uint32_t nextBase = next(m_Base);
#ifdef _DEBUG
		auto t = m_CurrentOrder;
#endif
		if (a.isMakMorton)
		{
			uint32_t nexta = next(a.vertex);
			improvement = m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(m_Base, a.vertex)
				+ m_Graph.GetEdge(a.vertex, nexta) - m_Graph.GetEdge(nexta, nextBase);
		  uint32_t newbase = nexta;
			uint32_t oldbase = m_Base;
#ifdef _DEBUG
			double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
			AddToFlipSequence(newbase, oldbase);
#ifdef _DEBUG
			double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
			double actualImprovement = oldDist - newDist;
			ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
			m_Base = newbase;
			if (IsWorse(delta + improvement))
				improvement += Step(level + 1, delta + improvement);
			m_Base = oldbase;
		}
		else
		{
			uint32_t preva = prev(a.vertex);
			improvement = m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(nextBase, a.vertex)
				+ m_Graph.GetEdge(preva, a.vertex) - m_Graph.GetEdge(preva, m_Base);
#ifdef _DEBUG
			double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
			AddToFlipSequence(nextBase, preva);
#ifdef _DEBUG
			double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
			double actualImprovement = oldDist - newDist;
			ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
			if (IsWorse(delta + improvement))
				improvement += Step(level + 1, delta + improvement);
		}
		
		if (IsImprovement(delta + improvement))
			return improvement;
		else
		{
			DeleteFromFlipSequence();
#ifdef _DEBUG
			auto q = m_CurrentOrder;
			for (uint32_t i = 0; i < m_CurrentOrder.size(); i++)
				ASSERT(q[i] == t[i]);
#endif
			i++;
		}
	}

	return 0.0;
}

void LKSearch::PerformFlipToCurrentTour(Flip flip)
{
	uint32_t p = flip.orderFrom;
	uint32_t q = flip.orderTo;

	if (p <= q)
		std::reverse(m_CurrentOrder.begin() + p, m_CurrentOrder.begin() + q + 1);
	else
		std::reverse(m_CurrentOrder.begin() + q + 1, m_CurrentOrder.begin() + p);

	UpdateOrderFromVertex();
}

void LKSearch::AddToFlipSequence(uint32_t from, uint32_t to)
{
	Flip flip = { m_CurrentOrderFromVertex[from], m_CurrentOrderFromVertex[to], from, to };
	m_FlipSequence.push(flip);
	PerformFlipToCurrentTour(flip);
}

void LKSearch::DeleteFromFlipSequence()
{
	LKSearch::Flip flip = m_FlipSequence.top();
	m_FlipSequence.pop();
	PerformFlipToCurrentTour(flip);
}

LKSearch::SearchResult LKSearch::Search(uint32_t base, const std::vector<uint32_t>& initialTour)
{
	m_CurrentOrder = initialTour;
	UpdateOrderFromVertex();

	m_Base = base;
	double improvement = Step(1, 0);
	/*
	* else TODO: do alternate_step
	*/

	return { m_CurrentOrder, improvement };
}

std::vector<uint32_t> LKSearch::LinKernighan(const TSPpermutation& initialTour)
{
	std::vector<uint32_t> lk_tour = initialTour.order;

	std::set<uint32_t> markedVertices;
	for (uint32_t i = 0; i < initialTour.order.size(); i++)
		markedVertices.insert(i);

	while (!markedVertices.empty())
	{
		uint32_t markedVertex = *(markedVertices.begin());
		LKSearch::SearchResult newOrder = Search(markedVertex, lk_tour);
		if (IsImprovement(newOrder.improvement))
		{
			while (!m_FlipSequence.empty())
			{
				Flip flip = m_FlipSequence.top();
				markedVertices.insert(flip.vertexFrom);
				markedVertices.insert(flip.vertexTo);
				m_FlipSequence.pop();
			}
#ifdef _DEBUG
			double newDist = m_Graph.calculateDistByOrder(newOrder.new_Tour);
			double oldDist = m_Graph.calculateDistByOrder(lk_tour);
			ASSERT(newDist < oldDist);
#endif _DEBUG
			lk_tour = newOrder.new_Tour;
		}
		else
		{
			markedVertices.erase(markedVertex);
		}
	}

	return lk_tour;
}
