#include "pch.h"
#include "threeSATinstance.h"


ThreeSATInstance::ThreeSATInstance(int _numberOfClauses, int _numberOfVariables, std::vector<int> _clauseLiterals)
{
	numberOfClauses = _numberOfClauses;
	numberOfVariables = _numberOfVariables;
	allClauseLiterals = _clauseLiterals;
	variableInteractionGraph = nullptr;
}

uint32_t ThreeSATInstance::GetSatisfiedClauses(const std::vector<bool>& variableAssignments) const
{
	ASSERT(variableAssignments.size() == numberOfVariables);
	uint32_t count = 0;
	for (uint32_t i = 0; i < numberOfClauses; i++)
	{
		bool satisfied = false;
		uint32_t offset = i * 3;
		int literal1 = allClauseLiterals[offset];
		int literal2 = allClauseLiterals[offset + 1];
		int literal3 = allClauseLiterals[offset + 2];
		bool assignment1 = variableAssignments[std::abs(literal1) - 1];
		bool assignment2 = variableAssignments[std::abs(literal2) - 1];
		bool assignment3 = variableAssignments[std::abs(literal3) - 1];

		auto IsNonNegated = [](int literal) {
			ASSERT(literal != 0);
			return literal > 0;
		};
		
		if ((IsNonNegated(literal1) == assignment1) || (IsNonNegated(literal2) == assignment2) || (IsNonNegated(literal3) == assignment3))
			count++;
	}

	return count;
}

void ThreeSATInstance::GenerateVariableInteractionGraph() const
{
	variableInteractionGraph = std::make_unique<UndirectedGraph>(numberOfVariables);

	for (uint32_t i = 0; i < numberOfClauses; i++)
	{
		uint32_t offset = i * 3;
		uint32_t var1 = std::abs(allClauseLiterals[offset]) - 1;
		uint32_t var2 = std::abs(allClauseLiterals[offset + 1]) - 1;
		uint32_t var3 = std::abs(allClauseLiterals[offset + 2]) - 1;
		variableInteractionGraph->addEdge(var1, var2);
		variableInteractionGraph->addEdge(var1, var3);
		variableInteractionGraph->addEdge(var2, var3);
	}
}

const UndirectedGraph& ThreeSATInstance::GetVariableInteractionGraph() const
{
	if (variableInteractionGraph == nullptr)
		GenerateVariableInteractionGraph();

	ASSERT(variableInteractionGraph != nullptr);
	return *variableInteractionGraph;
}