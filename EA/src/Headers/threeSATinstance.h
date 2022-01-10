#ifndef THREESATINSTANCE_H
#define THREESATINSTANCE_H

class ThreeSATInstance
{
public:
	ThreeSATInstance(int _numberOfClauses, int _numberOfVariables, std::vector<int> _clauseLiterals);
	int numberOfClauses;
	int numberOfVariables;
	std::vector<int> allClauseLiterals;
	uint32_t GetSatisfiedClauses(const std::vector<bool>& variableAssignments) const;
};
#endif