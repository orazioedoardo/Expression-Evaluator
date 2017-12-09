#include <iostream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

#define PI_VALUE "3.1415926535897932385"
#define  E_VALUE "2.7182818284590452354"

/*-------------------------------------------------------------*/

string help = R"(Available operators: + addition
                     - subtraction
                     * multiplication
                     / division
                     ^ exponentiation
                     ! factorial
                     % modulo

Available functions: abs(x) absolute value
                    sqrt(x) square root
                      ln(x) natural logarithm
                   log(x,y) logarithm in base x of y
                     sin(x) sine of angle in radians
                     cos(x) cosine of angle in radians
                     tan(x) tangent of angle in radians
                    asin(x) arcsine of angle in radians
                    acos(x) arccosine of angle in radians
                    atan(x) arctangent of angle in radians

Available parenthesis: { [ ( ) ] }

Available constants: pi ratio of a circumference to the diameter
                      e Euler’s constant

Other: -h to print this help)";

/*-------------------------------------------------------------*/

typedef double (*calc1)(double);
typedef double (*calc2)(double, double);
typedef vector <pair<string, string>> pairs;

/*-------------------------------------------------------------*/

double add (double x, double y){return x+y;}
double sub (double x, double y){return x-y;}
double mul (double x, double y){return x*y;}
double div (double x, double y){return x/y;}
double logb(double x, double y){return log(y)/log(x);}
double neg (double x){return -x;}
double fact(double x){return tgamma(x+1);}

/*-------------------------------------------------------------*/

void fail();
template <typename T> void push(T, vector <T> &);
template <typename T> void pop(T &, vector <T> &);
unsigned get_prec(char);
bool is_num(char);
bool is_func(char);
void replace_unary(char, char, string &);
void replace(const pairs &, string &);
string get_str(unsigned &, const string &);
double get_double(unsigned &, const string &);
string infix_to_postfix(const string &);
double evaluate_postfix(const string &);

/*-------------------------------------------------------------*/

map <char, calc1> map1 = {{'a',  abs}, {'b',  log}, {'c', sqrt}, {'d', asin},
                          {'f', acos}, {'g', atan}, {'h',  sin}, {'i',  cos},
                          {'j',  tan}, {'k',  neg}, {'l', fact}};

map <char, calc2> map2 = {{'+', add}, {'-',  sub}, {'*',  mul}, {'/', div},
                          {'^', pow}, {'%', fmod}, {'m', logb}};

pairs func = {{"abs",  "a"}, {"ln",   "b"}, {"sqrt", "c"}, {"asin", "d"},
              {"acos", "f"}, {"atan", "g"}, {"sin",  "h"}, {"cos",  "i"},
              {"tan",  "j"}, {"log",  "m"}};

pairs other = {{"!", "l"}, {"[", "("}, {"]", ")"}, {"{", "("}, {"}", ")"},
               {"pi", PI_VALUE}, {"e", E_VALUE}};

/*-------------------------------------------------------------*/

int main(int argc, char * argv[]){

	string in, rpn;
	double res;
	
	if (argc >= 2){
		if (strcmp(argv[1], "-h") == 0){
			cout << help << endl;
			return 0;
		} else {
			in = argv[1];
		}
	} else {
		cout << "Expression: ";
		getline(cin, in);
	}

	if (in.length() == 0) fail();

	replace(func, in);
	replace(other, in);

	replace_unary('-', 'k', in);
	replace_unary('+', 0, in);

	rpn = infix_to_postfix(in);
	res = evaluate_postfix(rpn);

    cout << "Result: " << setprecision(numeric_limits <double> ::digits10 + 1);
	cout << res << endl;
}

/*-------------------------------------------------------------*/

void fail(){
	cout << "Invalid expression (use -h to show the help)" << endl;
	exit(1);
}

template <typename T> void push(T o, vector <T> &s){
	s.push_back(o);
}

template <typename T> void pop(T &o, vector <T> &s){
	o = s.back();
	s.pop_back();
}

unsigned get_prec(char c){
	switch(c){
		case 'l': return 5;
		case 'k': return 4;
		case '^': return 4;
		case '/': return 3;
		case '*': return 3;
		case '%': return 2;
		case '-': return 1;
		case '+': return 1;
		default : return 0;
	}
}

bool is_num(char c){
	return ((c >= 48 && c <= 57) || c == '.') ? true : false;
}

bool is_func(char c){
	string o;
	for (unsigned w = 0; w < func.size(); w++){
		o = func.at(w).second;
		if (c == o.at(0)) return true;
	}
	return false;
}

void replace_unary(char from, char to, string &m){
    char u;
	for (unsigned i = 0; i < m.length(); i++){
		if (m.at(i) == ' '){
			m.erase(i--, 1);
		} else if (m.at(i) == from){
            if (i == 0){
                m.at(i) = to;
            } else {
    			u = m.at(i-1);
    			if ((get_prec(u) >= 1 && get_prec(u) <= 4) || (u == '(') || (u == ',')){
    				m.at(i) = to;
                }
			}
		}
	}
}

void replace(const pairs &v, string &s){
	int begin;
	string src, dst;
	for (unsigned w = 0; w < v.size(); w++){
		src = v.at(w).first;
		dst = v.at(w).second;
		for (unsigned i = 0; i < s.length(); i++){
			begin = s.find(src, i);
			if (begin == -1) break;
			s.replace(begin, src.length(), dst);
		}
	}
}

string get_str(unsigned &end, const string &v){
    string tot;
	unsigned begin = end;
	while ((end != v.length()-1) && is_num(v.at(end+1))) end++;
	tot = v.substr(begin, end-begin+1);
	return tot;
}

double get_double(unsigned &p, const string &v){
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

string infix_to_postfix(const string &in){

    string r1, r2, rpn;
    vector <string> op_stack, rpn_stack;

    for (unsigned i = 0; i < in.length(); i++){

		r1 = in.at(i);

		if (is_num(r1.at(0))){

			r1 = get_str(i, in);
			push(r1, rpn_stack);

		} else {

			if ((get_prec(r1.at(0)) >= 1) && (get_prec(r1.at(0)) <= 3)){

				for (string i : op_stack){
					r2 = op_stack.back();
					if (get_prec(r1.at(0)) <= get_prec(r2.at(0))){
						pop(r2, op_stack);
						push(r2, rpn_stack);
					}
				}
				push(r1, op_stack);

			} else if (get_prec(r1.at(0)) >= 4){

				for (string i : op_stack){
					r2 = op_stack.back();
					if (get_prec(r1.at(0)) < get_prec(r2.at(0))){
						pop(r2, op_stack);
						push(r2, rpn_stack);
					}
				}
				push(r1, op_stack);

			} else if ((r1.at(0) == '(') || is_func(r1.at(0))){

				push(r1, op_stack);

			} else if (r1.at(0) == ')'){

				for (string i : op_stack){
					r2 = op_stack.back();
					if (r2.at(0) != '('){
						pop(r2, op_stack);
						push(r2, rpn_stack);
					}
				}

				if (op_stack.empty() || (r2.at(0) != '(')) fail();
				pop(r2, op_stack);

				if (!op_stack.empty()){
					r2 = op_stack.back();
					if (is_func(r2.at(0))){
						pop(r2, op_stack);
						push(r2, rpn_stack);
					}
				}

			} else if (r1.at(0) == ','){

				for (string i : op_stack){
					r2 = op_stack.back();
					if (r2.at(0) != '('){
						pop(r2, op_stack);
						push(r2, rpn_stack);
					}
				}

				if (op_stack.empty() || (r2.at(0) != '(')) fail();
			}
		}
	}

	for (string i : op_stack){
		r2 = op_stack.back();
		if ((r2.at(0) == '(') || (r2.at(0) == ')')) fail();
		pop(r1, op_stack);
		push(r1, rpn_stack);
	}

	for (string i : rpn_stack) rpn += i + " ";
    return rpn;
}

double evaluate_postfix(const string &rpn){

    double l1, l2;
    vector <double> out_stack;

    for (unsigned i = 0; i < rpn.length(); i++){

		if (is_num(rpn.at(i))){

			l1 = get_double(i, rpn);
			push(l1, out_stack);

		} else {

			if (map1.find(rpn.at(i)) != map1.end()){

				if (out_stack.size() < 1) fail();
				pop(l1, out_stack);
				calc1 op = map1.at(rpn.at(i));
				l1 = op(l1);
				push(l1, out_stack);

			} else if (map2.find(rpn.at(i)) != map2.end()){

				if (out_stack.size() < 2) fail();
				pop(l1, out_stack);
				pop(l2, out_stack);
				calc2 op = map2.at(rpn.at(i));
				l2 = op(l2, l1);
				push(l2, out_stack);
			}
		}
	}

	if (out_stack.size() != 1) fail();
    return out_stack.back();
}
