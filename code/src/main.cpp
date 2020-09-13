#include <iostream>
#include <map>

#include "natural_deduction/types.h"
#include "first_order_logic/parser.h"
#include "natural_deduction/solver.h"
#include "natural_deduction/solverparser.h"

int main()
{
	std::string input;
	ND::Solver solver;
	ND::SolverParser parser(solver);

	std::string userInput;

	std::cout << "Welcome! If you need any help refer to the readme file." << std::endl;
	while (true)
	{
		std::getline(std::cin, userInput);
		if (!parser.Parse(userInput))
		{
			std::cout << "Error occurred: " << parser.GetError() << std::endl;
		}
	}
	return 0;
}