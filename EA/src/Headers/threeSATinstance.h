#ifndef THREESATINSTANCE_H
#define THREESATINSTANCE_H
#include "undirectedGraph.h"

class ThreeSATInstance
{
	mutable std::unique_ptr<UndirectedGraph> variableInteractionGraph;
	void GenerateVariableInteractionGraph() const;
public:
	ThreeSATInstance(int _numberOfClauses, int _numberOfVariables, std::vector<int> _clauseLiterals);
	int numberOfClauses;
	int numberOfVariables;
	std::vector<int> allClauseLiterals;
	uint32_t GetSatisfiedClauses(const std::vector<bool>& variableAssignments) const;
	const UndirectedGraph& GetVariableInteractionGraph() const;
};
#endif