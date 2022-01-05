#include "pch.h"
#include "lksearch.h"
#include <array>

LKSearch::LKSearch(const Graph& graph, const TSPpermutation& initialTour)
  : m_Graph(graph), m_InitialTour(initialTour)
{
	m_CurrentOrder = m_InitialTour.order;
	m_CurrentOrderFromVertex.resize(m_CurrentOrder.size());
	UpdateOrderFromVertex();
}

void LKSearch::UpdateOrderFromVertex()
{
	for (uint32_t i = 0; i < m_CurrentOrder.size(); i++)
		m_CurrentOrderFromVertex[m_CurrentOrder[i]] = i;
}

std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> LKSearch::GetLKOrdering(double delta)
{
	std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> lkOrdering;

	const auto& neighborsBase = m_Graph.GetNearNeighborsOf(m_Base);
	const auto& neighborsBaseNext = m_Graph.GetNearNeighborsOf(next(m_Base));

	double baseToNextBase = m_Graph.GetEdge(m_Base, next(m_Base));
	for (const Graph::VertexDist& a : neighborsBaseNext)
	{
		if (delta + baseToNextBase - a.distance > 0)
		{
			//It is promising so maximize c(prev(a),a) - c(next(base), a), where a is candidate
			double reward = m_Graph.GetEdge(prev(a.vertex), a.vertex) - a.distance;
			lkOrdering.emplace(a.vertex, reward, false);
		}
	}

	//Mak-morton moves
	for (const Graph::VertexDist& a : neighborsBase)
	{
		if (delta + baseToNextBase - a.distance > 0)
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
	return level > 2 ? 1 : 5;
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

		if (a.isMakMorton)
		{
			uint32_t nexta = next(a.vertex);
			improvement = m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(m_Base, a.vertex)
				+ m_Graph.GetEdge(a.vertex, nexta) - m_Graph.GetEdge(nexta, nextBase);
		  uint32_t newbase = nexta;
			uint32_t oldbase = m_Base;
			AddToFlipSequence({newbase, oldbase});
			m_Base = newbase;
			improvement += Step(level + 1, delta + improvement);
			m_Base = oldbase;
		}
		else
		{
			uint32_t preva = prev(a.vertex);
			improvement = m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(nextBase, a.vertex)
				+ m_Graph.GetEdge(preva, a.vertex) - m_Graph.GetEdge(preva, m_Base);
			AddToFlipSequence({ nextBase, preva });
			improvement += Step(level, delta + improvement);
		}
		
		if (improvement + delta < 0)
			return improvement;
		else
		{
			DeleteFromFlipSequence();
			i++;
		}
	}

	return 0.0;
}

void LKSearch::PerformFlipToCurrentTour(Flip flip)
{
	uint32_t p = m_CurrentOrderFromVertex[flip.start];
	uint32_t q = m_CurrentOrderFromVertex[flip.end];

	if (p < q)
		std::reverse(m_CurrentOrder.begin() + p, m_CurrentOrder.begin() + q + 1);
	else
		std::reverse(m_CurrentOrder.begin() + q, m_CurrentOrder.begin() + p + 1);

	UpdateOrderFromVertex();
}

void LKSearch::AddToFlipSequence(Flip flip)
{
	m_FlipSequence.push(flip);
	PerformFlipToCurrentTour(flip);
}

void LKSearch::DeleteFromFlipSequence()
{
	LKSearch::Flip flip = m_FlipSequence.top();
	m_FlipSequence.pop();
	PerformFlipToCurrentTour(flip);
}

bool LKSearch::Search(uint32_t base)
{
	m_Base = base;
	

	return true;
}
