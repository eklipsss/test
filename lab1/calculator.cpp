#include "calculator.h"

Calculator::Calculator() 
{
	base_operations();
	try {
		load_dll();
	}
	catch (std::exception& err) {
		std::cout << err.what() << std::endl;
	}
	print_functions();
}

Calculator::~Calculator() 
{
	while (!cur_operations.empty()) delete cur_operations.top();
	for (auto& oper : operations) delete oper;
	for (auto& dll : dlls) FreeLibrary(dll);
}

void Calculator::base_operations()
{
	Operation* add = new Operation("+", 1, 1, [](std::stack<double>& args) {
		if (args.size() < 2)
			throw std::exception("Error: There must be 2 arguments for addition\n");
		double arg1 = args.top();
		args.pop();
		//std::cout << arg1 << "+" << args.top() << std::endl;
		double arg2 = args.top() + arg1;
		args.top() = arg2;
		});

	Operation* sub = new Operation("-", 1, 1, [](std::stack<double>& args) {
		if (args.size() < 2) 
			throw std::exception("Error: There must be 2 arguments for substract\n");
		double arg1 = args.top();
		args.pop();
		//std::cout << args.top() << "-" << arg1 << std::endl;
		double arg2 = args.top() - arg1;
		args.top() = arg2;
		});

	Operation* mul = new Operation("*", 2, 1, [](std::stack<double>& args) {
		if (args.size() < 2)
			throw std::exception("Error: There must be 2 arguments for multiplication\n");
		double arg1 = args.top();
		args.pop();
		//std::cout << arg1 << "*" << args.top() << std::endl;
		double arg2 = args.top() * arg1;
		args.top() = arg2;

		});

	Operation* div = new Operation("/", 2, 1, [](std::stack<double>& args) {
		if (args.size() < 2)
			throw std::exception("Error: There must be 2 arguments for division\n");
		if (args.top() == 0)
			throw std::exception("Error: The second argument can't be 0\n");
		double arg1 = args.top();
		args.pop();
		//std::cout << args.top() << "/" << arg1 << std::endl;
		double arg2 = args.top() / arg1;
		args.top() = arg2;
		});

	Operation* left_bracket = new Operation("(", 4, 0, nullptr);
	Operation* right_bracket = new Operation(")", 4, 0, nullptr);

	operations.push_back(add);
	operations.push_back(sub);
	operations.push_back(mul);
	operations.push_back(div);
	operations.push_back(left_bracket);
	operations.push_back(right_bracket);

}

void Calculator::load_dll() {
	std::cout << std::endl << "Load DLL:" << std::endl;
	std::filesystem::path plugins_path = std::filesystem::current_path().parent_path();
	//std::filesystem::path plugins_path = std::filesystem::current_path();
	plugins_path /= "plugins";
	if (!std::filesystem::exists(plugins_path))
		throw std::exception("Error: Folder <plagins> not found\n");

	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	for (const auto& file : recursive_directory_iterator("..\\plugins")) {
		if (file.path().extension() != ".dll")
			continue;
		std::string filename = file.path().string();
		std::cout << filename << std::endl;
		HMODULE dll = LoadLibraryA(filename.c_str());
		//if (!dll)
			//throw std::exception("Error: Files in directory is not dll format");
		dlls.push_back(dll);
		try {
			import_functions(dll);
		}
		catch (std::exception& err) {
			std::cout << err.what() << std::endl;
		}
	}
	if (dlls.empty()) {
		throw std::exception("Attention: No dlls in folder\n");
	}
	std::cout << std::endl;
}

void Calculator::import_functions(HMODULE dll) {	
	if (GetProcAddress(dll, "type") == NULL || GetProcAddress(dll, "priority") == NULL || GetProcAddress(dll, "binary") == NULL || GetProcAddress(dll, "operation") == NULL) {
		throw std::exception("Error: Incorrect dll\n");
	}
	else {
		std::function<std::string(void)> type = (std::string(*) (void)) GetProcAddress(dll, "type");
		std::function<int(void)>  priority = (int (*) (void))GetProcAddress(dll, "priority");
		std::function<bool(void)>  binary = (bool (*) (void))GetProcAddress(dll, "binary");

		std::function func_realization = [dll, binary](std::stack<double>& args) {
			if (args.size() == 0)
				throw std::exception("Error: The operation is performed on at least one operand\n");
			if (binary() == 0 && args.size() >= 1) {
				std::function<double(double)> oper = (double (*) (double))GetProcAddress(dll, "operation");
				args.top() = oper(args.top());
				return args.top();
			}
			else if (binary() == 1 && args.size() >= 2) {
				std::function<double(double, double)> oper = (double (*) (double, double))GetProcAddress(dll, "operation");
				double arg1 = args.top();
				args.pop();
				double arg2 = oper(args.top(), arg1);
				args.top() = arg2;
				return arg2;
			}
			else
				throw std::exception("Error: Mismatch between function and number of args\n");
		};
		Operation* func = new Operation(type(), priority(), binary(), func_realization);

		operations.push_back(func);
		//std::cout << func->get_type() << std::endl;
	}
}


void Calculator::print_functions() {
	std::cout << "Functions:  ";
	for (auto& oper : operations) std::cout<< oper->get_type() <<"  ";
	std::cout << std::endl << std::endl;
}

void Calculator::put_expression()
{
	std::cout << "Expression:  ";
	std::getline(std::cin, expression);
	delete_spaces();
}

void Calculator::delete_spaces()
{
	for (int i = 0; i < expression.length(); i++) {
		if (expression[i] == ' ') {
			expression.erase(expression.begin() + i);
			i--;
		}
	}
}

std::string Calculator::solve()
{
	char c;
	std::string buf;
	for (int i = 0; i < expression.size(); i++) {
		c = expression[i];
		if (is_number(c) || c == '-' && ( i == 0 || i!=0 && expression[i-1] == '(')) { //число
			int j = i;
			bool point = false;
			while (is_number(c) || c == '-' && (j == 0 || j != 0 && expression[j - 1] == '(') || c == '.') {
				if (c == '.') {
					if (point) throw std::exception("Error: A number can't have two separators (dots)\n");
					point = true;
				}
				buf += c;
				j++;
				c = expression[j];
			}
			//std::cout << buf << std::endl;
			double num = std::stod(buf);
			numbers.push(num);
			i += buf.size() - 1;
			buf = "";
		}
		else { //не число
			//std::cout << c << std::endl;
			bool oper_exists = false;
			for (auto const& oper : operations) { // проверяем совпадение элементов строки с названиями доступных операций
				if (match(i,oper)) { // если соответствующая операция нашлась
					oper_exists = true;
					i += oper->get_type().size() - 1;

					if ((cur_operations.empty() || oper->get_type() == "(") && oper->get_type() != ")") cur_operations.push(oper);
					else if (oper->get_type() == ")") if (!math()) throw std::exception("Error: The expression was entered incorrectly\n");
					else {
						if (oper->get_priority() > cur_operations.top()->get_priority() || cur_operations.top()->get_type() == "(") 
							cur_operations.push(oper);
						else {
							if (!math()) throw std::exception("Error: The expression was entered incorrectly\n");
							cur_operations.push(oper);
						}
					}
					break;
				}
			}
			if (!oper_exists) {
				throw std::exception("Error: Operation or symbol is not correct\n");
				//exit(1);
			}
		}

	}
	if (!math()) throw std::exception("Error: The expression was entered incorrectly\n");
	if (numbers.size() == 1 && cur_operations.size() == 0) {
		std::string str = std::to_string(numbers.top());
		clear_stacks();
		return str;
	}
	else {
		clear_stacks();
		throw std::exception("Error: The expression was entered incorrectly\n");
	}
}

bool Calculator::math() {
	while (!cur_operations.empty()) {
		if (cur_operations.top()->get_type() == "(") cur_operations.pop();
		else {
			for (auto const& oper : operations) {
				if (oper->get_type() == cur_operations.top()->get_type()) {
					try {
						oper->func(numbers);
					}
					catch (std::exception& err) {
						std::cout << err.what();
						return 0;
						//exit(1);
					}
					cur_operations.pop();
					break;
				}
			}
		}
	}
	return 1;
}

bool Calculator::match(int i, Operation* oper) 
{
	bool match = true;
	for (int k = 0; k < oper->get_type().size(); ++k) {
		//std::cout << oper->get_type() << std::endl;
		if (oper->get_type()[k] != expression[i + k]) {
			match = false;
			break;
		}
	}
	return match;
}

bool Calculator::is_number(char c) 
{
	return (c >= '0' && c <= '9') ? true : false;
}

void Calculator::clear_stacks() {
	while (!numbers.empty())
		numbers.pop();
	while (!cur_operations.empty())
		delete cur_operations.top();
}

