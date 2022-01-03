#include "pch.h"
#include "threeSATinstance.h"


ThreeSATInstance::ThreeSATInstance(int _numberOfClauses, int _numberOfVariables, std::vector<int> _clauses)
{
	numberOfClauses = _numberOfClauses;
	numberOfVariables = _numberOfVariables;
	clauses = _clauses;
}
