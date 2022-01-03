#ifndef THREESATINSTANCE_H
#define THREESATINSTANCE_H

class ThreeSATInstance
{
public:
	ThreeSATInstance(int _numberOfClauses, int _numberOfVariables, std::vector<int> _clauses);
	int numberOfClauses;
	int numberOfVariables;
	std::vector<int> clauses;

};
#endif