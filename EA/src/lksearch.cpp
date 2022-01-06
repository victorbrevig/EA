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

bool LKSearch::IsOnPath(uint32_t vertex, uint32_t from, uint32_t to)
{
	uint32_t v = m_CurrentOrderFromVertex[vertex];
	uint32_t f = m_CurrentOrderFromVertex[from];
	uint32_t t = m_CurrentOrderFromVertex[to];

	ASSERT(v != f);
	ASSERT(v != t);
	ASSERT(f != t);

	if (f < t && v >= f && v <= t)
		return true;

	if (t < f && (v >= f || v <= t))
		return true;

	return false;
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
		if (!IsImprovement(delta + baseToNextBase - a.distance))
			break;
		//It is promising so maximize c(prev(a),a) - c(next(base), a), where a is candidate
		double reward = m_Graph.GetEdge(prev(a.vertex), a.vertex) - a.distance;
		lkOrdering.emplace(a.vertex, reward, false);
	}

	//Mak-morton moves
	for (const Graph::VertexDist& a : neighborsBase)
	{
		if (next(a.vertex) == m_Base)
			continue;
		if (!IsImprovement(delta + baseToNextBase - a.distance))
			break;
		//It is promising so maximize c(prev(a),a) - c(next(base), a), where a is candidate
		double reward = m_Graph.GetEdge(a.vertex, next(a.vertex)) - a.distance;
		lkOrdering.emplace(a.vertex, reward, true);
	}

	return lkOrdering;
}

std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> LKSearch::GetAOrdering()
{
	std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> AOrdering;

	uint32_t nextBase = next(m_Base);

	const auto& neighbors = m_Graph.GetNearNeighborsOf(nextBase);

	double baseToNextBase = m_Graph.GetEdge(m_Base, nextBase);
	for (const Graph::VertexDist& a : neighbors)
	{
		if (prev(a.vertex) == nextBase || a.vertex == m_Base)
			continue;
		if (!IsImprovement(baseToNextBase - a.distance))
			break;
		//It is promising so maximize c(next(a),a) - c(next(base), a), where a is candidate
		double reward = m_Graph.GetEdge(next(a.vertex), a.vertex) - a.distance;
		AOrdering.emplace(a.vertex, reward, false);
	}

	return AOrdering;
}

std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> LKSearch::GetBOrdering(uint32_t a1)
{
	std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> BOrdering;

	uint32_t nextBase = next(m_Base);

	const auto& neighbors = m_Graph.GetNearNeighborsOf(a1);

	for (const Graph::VertexDist& b : neighbors)
	{
		if (b.vertex == m_Base || b.vertex == nextBase || b.vertex == prev(a1))
			continue;

		if (b.distance < m_Graph.GetEdge(prev(a1), a1) + m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(nextBase, prev(a1)))
		{
			double reward = m_Graph.GetEdge(next(b.vertex), b.vertex) - b.distance;
			BOrdering.emplace(b.vertex, reward, false);

			if (prev(b.vertex) == a1 || prev(b.vertex) == nextBase)
				continue;
			double reward2 = m_Graph.GetEdge(prev(b.vertex), b.vertex) - b.distance;
			BOrdering.emplace(b.vertex, reward, true);
		}
	}

	return BOrdering;
}

std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> LKSearch::GetDOrdering(uint32_t a1, uint32_t b1)
{

	std::priority_queue<LKSearch::PromissingVertex, std::vector<LKSearch::PromissingVertex>, LKSearch::Compare> DOrdering;

	uint32_t nextBase = next(m_Base);
	uint32_t a = prev(a1);
	uint32_t b = next(b1);

	const auto& neighbors = m_Graph.GetNearNeighborsOf(b1);

	for (const Graph::VertexDist& d : neighbors)
	{
		if (d.vertex == m_Base || d.vertex == nextBase || d.vertex == a || d.vertex == a1 || d.vertex == b)
			continue;
		if (IsOnPath(d.vertex, nextBase, prev(a1)))
		{
			if (m_Graph.GetEdge(b1, d.vertex) < m_Graph.GetEdge(b, b1) 
				+ m_Graph.GetEdge(m_Base, nextBase) - m_Graph.GetEdge(nextBase, a)
				+ m_Graph.GetEdge(a, a1) - m_Graph.GetEdge(a1, b))
			{
				uint32_t d1 = next(d.vertex);
				double reward = m_Graph.GetEdge(d1, d.vertex) - d.distance;
				DOrdering.emplace(d.vertex, reward, false);

				d1 = prev(d.vertex);
				double reward2 = m_Graph.GetEdge(d1, d.vertex) - d.distance;
				DOrdering.emplace(d.vertex, reward, true);
			}
		}
	}

	return DOrdering;
}

uint32_t LKSearch::Breadth(uint32_t level)
{
	switch (level)
	{
	case 1:
		return 10;
	case 2:
		return 5;
	case 3:
		return 1;
	case 4:
		return 1;
	case 5:
		return 1;
	case 6:
		return 1;
	default:
		return 0;
	}
}

uint32_t LKSearch::BreadthA()
{
	return Breadth(1);
}

uint32_t LKSearch::BreadthB()
{
	return Breadth(1);
}

uint32_t LKSearch::BreadthD()
{
	return Breadth(1);
}

double LKSearch::StepAlternate()
{
	uint32_t s1 = next(m_Base);

	uint32_t i = 1;
	auto AOrdering = GetAOrdering();
	while (!AOrdering.empty() && i <= BreadthA())
	{
		LKSearch::PromissingVertex a = AOrdering.top();
		AOrdering.pop();
		uint32_t a1 = next(a.vertex);

		uint32_t j = 1;
		auto BOrdering = GetBOrdering(a1);
		while (!BOrdering.empty() && j <= BreadthB())
		{
			LKSearch::PromissingVertex b = BOrdering.top();
			BOrdering.pop();
			uint32_t b1 = b.b1IsPrev ? prev(b.vertex) : next(b.vertex);

			if (IsOnPath(b.vertex, s1, a.vertex))
			{
				double improvement = 0.0;
#ifdef _DEBUG
				double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
				if (!b.b1IsPrev)
				{
					improvement  = AddToFlipSequence(s1, b.vertex);
					improvement += AddToFlipSequence(b.vertex, a.vertex);
				}
				else
				{
					improvement  = AddToFlipSequence(s1, a.vertex);
					improvement += AddToFlipSequence(b.vertex, s1);
					improvement += AddToFlipSequence(a.vertex, b1);
				}
#ifdef _DEBUG
				double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
				double actualImprovement = oldDist - newDist;
				ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
				improvement += Step(3, improvement);
				if (IsImprovement(improvement))
					return improvement;
				else
				{
					DeleteFromFlipSequence();
					DeleteFromFlipSequence();
					if (b.b1IsPrev)
						DeleteFromFlipSequence();
				}
			}
			else if (b.b1IsPrev)
			{
				uint32_t k = 1;
				auto DOrdering = GetDOrdering(a1, b1);
				while (!DOrdering.empty() && k <= BreadthD())
				{
					LKSearch::PromissingVertex d = DOrdering.top();
					DOrdering.pop();
					double improvement = 0.0;
					uint32_t d1 = b.b1IsPrev ? prev(d.vertex) : next(d.vertex);
#ifdef _DEBUG
					double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
					if (!d.d1IsPrev)
					{
						improvement  = AddToFlipSequence(s1, b1);
						improvement += AddToFlipSequence(b1, d1);
						improvement += AddToFlipSequence(a1, s1);
					}
					else
					{
						improvement  = AddToFlipSequence(s1, d1);
						improvement += AddToFlipSequence(d.vertex, a.vertex);
						improvement += AddToFlipSequence(a1, b1);
					}
#ifdef _DEBUG
					double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
					double actualImprovement = oldDist - newDist;
					ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
					improvement += Step(4, improvement);
					if (IsImprovement(improvement))
						return improvement;
					else
					{
						DeleteFromFlipSequence();
						DeleteFromFlipSequence();
						DeleteFromFlipSequence();
						k++;
					}
				}
			}
			j++;
		}
		i++;
	}

	return 0.0;
}

double LKSearch::Step(uint32_t level, double delta)
{
	if (IsImprovement(delta))
		return 0.0;

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
		  uint32_t newbase = nexta;
			uint32_t oldbase = m_Base;
#ifdef _DEBUG
			double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
			improvement = AddToFlipSequence(newbase, oldbase);
#ifdef _DEBUG
			double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
			double actualImprovement = oldDist - newDist;
			ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
			m_Base = newbase;
			improvement += Step(level + 1, delta + improvement);
			m_Base = oldbase;
		}
		else
		{
			uint32_t preva = prev(a.vertex);
#ifdef _DEBUG
			double oldDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
#endif
			improvement = AddToFlipSequence(nextBase, preva);

#ifdef _DEBUG
			double newDist = m_Graph.calculateDistByOrder(m_CurrentOrder);
			double actualImprovement = oldDist - newDist;
			ASSERT(std::abs(actualImprovement - improvement) < 0.001);
#endif
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
	{
		/*uint32_t a = p;
		uint32_t b = q;
		do
		{
			std::swap(m_CurrentOrder[a], m_CurrentOrder[b]);
			a = (a + 1) % m_CurrentOrder.size();
			b = (b == 0 ? (m_CurrentOrder.size() - 1) : (b - 1));
		} while (a > b);
		std::reverse(m_CurrentOrder.begin() + a, m_CurrentOrder.begin() + b + 1);*/
		std::reverse(m_CurrentOrder.begin() + q + 1, m_CurrentOrder.begin() + p);
	}

	UpdateOrderFromVertex();
}

double LKSearch::AddToFlipSequence(uint32_t from, uint32_t to)
{
	uint32_t oFrom = m_CurrentOrderFromVertex[from];
	uint32_t oTo = m_CurrentOrderFromVertex[to];
	bool noop = (oFrom == oTo || (oTo + 1) % m_CurrentOrder.size() == oFrom);

	double delta = noop ? 0.0 : (m_Graph.GetEdge(prev(from), from) - m_Graph.GetEdge(from, next(to))
		+ m_Graph.GetEdge(to, next(to)) - m_Graph.GetEdge(to, prev(from)));
	Flip flip = { oFrom, oTo, from, to };
	m_FlipSequence.push(flip);
	PerformFlipToCurrentTour(flip);

	return delta;
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
	if (IsImprovement(improvement))
		return { m_CurrentOrder, improvement };
	else
	{
		m_CurrentOrder = initialTour;
		UpdateOrderFromVertex();
		improvement = StepAlternate();
		if (IsImprovement(improvement))
			return { m_CurrentOrder, improvement };
	}

	return { std::vector<uint32_t>() , 0.0 };
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
