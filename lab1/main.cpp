#include "calculator.h"

int main()
{
	std::cout << "Calculator\n";
	Calculator calc;
	
	calc.put_expression();
	while (calc.check_expression() != "" || calc.check_expression() != "stop")
	{
		if (calc.check_expression() == "" || calc.check_expression() == "stop") break;
		try 
		{
			std::cout << "Result:  " << calc.solve() << std::endl << std::endl;
		}
		catch (std::exception& error)
		{
			std::cout << error.what() << std::endl;
		}
		calc.put_expression();
	}
	return 0;
}