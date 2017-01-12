#include <iostream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <vector>
#include <cmath>
#include <map>

#define PI_VALUE "3.1415926535897932385"
#define  E_VALUE "2.7182818284590452354"

using namespace std;

double add(double x, double y){return x+y;}
double sub(double x, double y){return x-y;}
double mul(double x, double y){return x*y;}
double div(double x, double y){return x/y;}
double logb(double x, double y){return log(y)/log(x);}
double neg(double x){return -1*x;}
double fact(double x){return tgamma(x+1);}

typedef double (*calc1)(double);
typedef double (*calc2)(double, double);
typedef vector <pair<string, string>> pairs;

//Map a character to a function pointer, map1 is used for functions and operators
//which take one double, map2 for those who take two doubles

map <char, calc1> map1 = {{'a',  abs}, {'b',  log}, {'c', sqrt}, {'d', asin},
                          {'f', acos}, {'g', atan}, {'h',  sin}, {'i',  cos},
                          {'j',  tan}, {'k',  neg}, {'l', fact}};
                            
map <char, calc2> map2 = {{'+', add}, {'-',  sub}, {'*',  mul}, {'/', div},
                          {'^', pow}, {'%', fmod}, {'m', logb}};

//Vectors of pairs containing substitutions

pairs func = {{"abs",  "a"}, {"ln",   "b"}, {"sqrt", "c"}, {"asin", "d"}, 
              {"acos", "f"}, {"atan", "g"}, {"sin",  "h"}, {"cos",  "i"},
              {"tan",  "j"}, {"log",  "m"}};

pairs other = {{"!", "l"}, {"[", "("}, {"]", ")"}, {"{", "("}, {"}", ")"},
               {"pi", PI_VALUE}, {"e", E_VALUE}};

vector <string> op_stack;
vector <string> rpn_stack;
vector <double> out_stack;

template <typename T> void push(T o, vector <T> &s){
	s.push_back(o);
}

template <typename T> void pop(T &o, vector <T> &s){
	o = s.back();
	s.pop_back();
}

int get_prec(char c){
	switch(c){
		case 'l': return 5; //Factorial operator
		case 'k': return 4; //Negation operator
		case '^': return 4;
		case '/': return 3;
		case '*': return 3;
		case '%': return 2;
		case '-': return 1;
		case '+': return 1;
		default: return 0;
	}
}

bool num(char c){
	return ((c >= 48 && c <= 57) || c == '.') ? true : false;
}

//Determine if a charater corresponds to a function

bool is_func(char c){
	string o;
	for (int w = 0; w < func.size(); w++){
		o = func.at(w).second;
		if (c == o.at(0)) return true;
	}
	return false;
}

//Replace unary operators '-' and '+' to prevent ambiguity

void replace_una(char from, char to, string &m){
	int p, i;
	if (m.at(0) == from) m.at(0) = to;
	for (i = 1; i < m.length(); i++){
		if (m.at(i) == from){
			for (p = i-1; p > 0; p--){
				if (m.at(p) != ' ') break;
			}
			char u = m.at(p);
			if ((get_prec(u) >= 1 && get_prec(u) <= 4) || (u == '(')){
				m.at(i) = to;
			}
		}
	}
}

//Replace occurrences of items of a vector in a given string

void replace(pairs v, string &s){
	int begin;
	string src, dst;
	for (int w = 0; w < v.size(); w++){
		src = v.at(w).first;
		dst = v.at(w).second;
		for (int i = 0; i < s.length(); i++){
			begin = s.find(src, i);
			if (begin == -1) break;
			s.replace(begin, src.length(), dst);
			i += begin;
		}  
	}
}

//Obtain a double (as a string) at a p offest inside a string 
//and move it to p + double length

string get_str(int &p, string v){
	string tot;
	for (int i = p; i < v.length(); i++){
		tot += v.at(i);
		if ((i == v.length()-1) || !num(v.at(i+1))){
			p = i;
			break;
		}
	}
	return tot;
}

void fail(){
	cout << "Invalid expression (use -h to show the help)" << endl;
	exit(1);
}

//Obtain a double at a p offest inside a string and move it to 
//p + double length

double get_double(int &p, string v){
	size_t i;
	double tot;
	try {
		tot = stod(&v.at(p), &i);
	} catch (invalid_argument){
		fail();
	}
	p += i;
	return tot;
}

void show_rpn(bool v){
	if (v){
		for (string i : rpn_stack) cout << i << " ";
		cout << "\t";
		for (string i : op_stack) cout << i << " ";
		cout << endl;
	}
}

void show_out(bool v){
	if (v){
		for (double i : out_stack) cout << i << " ";
		cout << endl;
	}
}

int main(int argc, char * argv[]){
	
	string r0, r1, r2;
	double l0, l1, l2;

	string in;
	cout << "Expression: ";
	getline(cin, in);

	if (in.length() == 0) fail();

	replace(func, in);
	replace(other, in);

	replace_una('-', 'k', in);
	replace_una('+', 0, in);

	bool v = false;
	if ((argc >= 2) && (strcmp(argv[1], "-v") == 0)) v = true;
	
	if (v) cout << "Input: " << in << endl;

	for (int i = 0; i < in.length(); i++){

		r0 = in.at(i);

		//If r0 is a number, push it onto rpn_stack

		if (num(r0.at(0))){

			r0 = get_str(i, in);
			push(r0, rpn_stack);
			show_rpn(v);

		} else {

			//If it isn't a number it's either an operator, a function, or
			//a parenthesis.

			//If r0 is left associative and its precedence is <= to that of
			//r1, mode r1 to rpn_stack (for every operator from op_stack)

			if ((get_prec(r0.at(0)) >= 1) && (get_prec(r0.at(0)) <= 3)){

				for (string i : op_stack){
					r1 = op_stack.back();
					if (get_prec(r0.at(0)) <= get_prec(r1.at(0))){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
				push(r0, op_stack);
				show_rpn(v);

			//If r0 is right associative and its precedence is < to that of
			//r1, mode r1 to rpn_stack (for every operator from op_stack)

			} else if (get_prec(r0.at(0)) >= 4){

				for (string i : op_stack){
					r1 = op_stack.back();
					if (get_prec(r0.at(0)) < get_prec(r1.at(0))){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
				push(r0, op_stack);
				show_rpn(v);

			//Push '(' and functions (as characters) to op_stack

			} else if ((r0.at(0) == '(') || is_func(r0.at(0))){

				push(r0, op_stack);
				show_rpn(v);

			} else if (r0.at(0) == ')'){

				//Move operators from op_stack to rpn_stack until '(' is found

				bool found = false;
				for (string i : op_stack){
					r1 = op_stack.back();
					if (r1.at(0) != '('){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}

				//Then pop it outside op_stack

				if (!op_stack.empty()){
					if (r1.at(0) == '('){
						found = true;
						pop(r1, op_stack);
						show_rpn(v);
					}
				}

				//Move functions from op_stack to rpn_stack

				if (!op_stack.empty()){
					r1 = op_stack.back();
					if (is_func(r1.at(0))){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}

				//If '(' hasn't been found, then there are mismatched
				//parenthesis

				if (!found) fail();

			} else if (r0.at(0) == ','){

				//Move operators from op_stack to rpn_stack

				for (string i : op_stack){
					r1 = op_stack.back();
					if (r1.at(0) != '('){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
			}
		}
	}

	//Move remaining operators from op_stack to rpn_stack, if parenthesis are found,
	//then there are mismatched ones

	for (string i : op_stack){
		r2 = op_stack.back();
		if ((r2.at(0) == '(') || (r2.at(0) == ')')) fail();
		pop(r1, op_stack);
		push(r1, rpn_stack);
	}

	string rpn;
	for (string i : rpn_stack) rpn += i + " ";

	if (v) cout << "RPN: " << rpn << endl;

	for (int i = 0; i < rpn.length(); i++){

		if (num(rpn.at(i))){

			//If l0 is a number, push it onto rpn_stack

			l0 = get_double(i, rpn);
			push(l0, out_stack); show_out(v);

		} else {

			//Find the operator or the function in either one of two maps,
			//check if there are enough numbers for the operation, pop operands
			//from out_stack, obtain the function pointer (calcN op), evaluate
			//and push the result back to out_stack

			if (map1.find(rpn.at(i)) != map1.end()){

				if (out_stack.size() < 1) fail();
				pop(l1, out_stack);
				calc1 op = map1.at(rpn.at(i));
				l1 = op(l1);
				push(l1, out_stack);
				show_out(v);

			} else if (map2.find(rpn.at(i)) != map2.end()){

				if (out_stack.size() < 2) fail();
				pop(l1, out_stack);
				pop(l2, out_stack);
				calc2 op = map2.at(rpn.at(i));
				l2 = op(l2, l1);
				push(l2, out_stack);
				show_out(v);
			}
		}
	}

	//The output stack should contain only one item, otherwise an error occured

	if (out_stack.size() != 1) fail();
	
	//Set the highest precision

	cout << "Result: " << setprecision(numeric_limits <double> ::digits10 + 1);
	cout << out_stack.back() << endl;
}
