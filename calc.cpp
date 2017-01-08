#include <iostream>
#include <cstring>
#include <iomanip>
#include <limits>
#include <vector>
#include <cmath>

#define PI_VALUE "3.1415926535897932385"
#define E_VALUE "2.7182818284590452354"

using namespace std;

vector <string> op_stack;
vector <string> rpn_stack;
vector <double> out_stack;

//Available functions along with their ids
vector <string> avail = {"abs", "ln", "sqrt", "asin", "acos", 
						 "atan", "sin", "cos", "tan", "log"};
vector <string> ids = {"a", "b", "c", "d", "f", 
					   "g", "h", "i", "j", "m"};

template <typename T> void push(T o, vector <T> &s){
	s.push_back(o);
}

template <typename T> void pop(T &o, vector <T> &s){
	o = s.back();
	s.pop_back();
}

//Operators' precedence
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

//Compare c against ids list
bool is_func(char c){
	string o;
	for (int w = 0; w < ids.size(); w++){
		o = ids.at(w);
		if (c == o.at(0)) return true;
	}
	return false;
}

void replace_una(char from, char to, string &m){
	char u;
	if (m.at(0) == from) m.at(0) = to;
	for (int i = 1; i < m.length(); i++){
		if (m.at(i) == from){
			for (int p = i-1; p > 0; p--){
				u = m.at(p);
				if ((get_prec(u) >= 1) || (u == '(')){
					m.at(i) = to;
					break;
				} else if (num(u)){
					break;
				}				
			}
		}
	}
}

//Replace functions names with their corresponding ids
void replace_func(string &s){
	int begin;
	string from, to;
	for (int w = 0; w < avail.size(); w++){
		from = avail.at(w);
		to = ids.at(w);
		for (int i = 0; i < s.length(); i++){
			begin = s.find(from, i);
			if (begin == -1){
				break;
			}
			s.replace(begin, from.length(), to);
			i += begin;
		}  
	}
}

//Generic string replacement
void replace_str(string from, string to, string &s){
	int begin;
	for (int i = 0; i < s.length(); i++){
		begin = s.find(from, i);
		if (begin == -1){
			break;
		}
		s.replace(begin, from.length(), to);
		i += begin;
	}  
}

string get_str(int &p, string v){
	string tot;
	v += " ";
	for (int i = p; num(v.at(i)); i++){
		tot += v.at(i);
		if (!num(v.at(i+1))){
			p = i;
			break;
		}
	}
	return tot;
}

double get_double(int &p, string v){
	size_t i;
	double tot = stod(&v.at(p), &i);
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
	
	string r0, r1;
	char c0, c1, c2;
	double l0, l1, l2;

	string in;
	cout << "Expression: ";
	getline(cin, in);

	replace_func(in);

	//Replace unary operators, es. +2 * -8 -> 2 * k8
	//It's necessary to prevent ambiguity with the '-' and '+' operator
	replace_una('-', 'k', in);
	replace_una('+', 0, in);

	replace_str("pi", PI_VALUE, in);
	replace_str("e", E_VALUE, in);

	replace_str("!", "l", in);

	replace_str("[", "(", in);
	replace_str("]", ")", in);
	replace_str("{", "(", in);
	replace_str("}", ")", in);

	bool v = false;
	if ((argc >= 2) && (strcmp(argv[1], "-v") == 0)){
		v = true;
	}
	
	if(v) cout << "Input: " << in << endl;

	for (int i = 0; i < in.length(); i++){

		r0 = in.at(i);
		c0 = r0.at(0);

		if (num(c0)){

			//Get the current number and move the index i
			//to i + length of the number
			r0 = get_str(i, in);
			push(r0, rpn_stack);
			show_rpn(v);

		} else {

			//Handle left associativity
			if ((get_prec(c0) >= 1) && (get_prec(c0) <= 3)){

				//While op_stack contains operators, if c0's precedence
				//is <= than c1's precedence, move r1 (operator) to rpn_stack
				for (int i = op_stack.size(); i > 0; i--){
					r1 = op_stack.back();
					c1 = r1.at(0);
					if (get_prec(c0) <= get_prec(c1)){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
				push(r0, op_stack);
				show_rpn(v);

			//Handle right associativity
			} else if (get_prec(c0) >= 4){

				//While op_stack contains operators, if c0's precedence
				//is < than c1's precedence, move r1 (operator) to rpn_stack
				for (int i = op_stack.size(); i > 0; i--){
					r1 = op_stack.back();
					c1 = r1.at(0);
					if (get_prec(c0) < get_prec(c1)){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
				push(r0, op_stack);
				show_rpn(v);

			} else if ((c0 == '(') || is_func(c0)){

				push(r0, op_stack);
				show_rpn(v);

			} else if (c0 == ')'){

				//Until at the top of op_stack a '(' is found, move
				//r1 (operator) to rpn_stack
				for (int i = op_stack.size(); i > 0; i--){
					r1 = op_stack.back();
					c1 = r1.at(0);
					if (c1 != '('){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
				pop(r1, op_stack);
				show_rpn(v);

				if (!op_stack.empty()){
					r1 = op_stack.back();
					c1 = r1.at(0);

					//If the top of op_stack contains a function, move
					//it to rpn_stack
					if (is_func(c1)){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}

			} else if (c0 == ','){

				//Same as if c0 was a '('
				for (int i = op_stack.size(); i > 0; i--){
					r1 = op_stack.back();
					c1 = r1.at(0);
					if (c1 != '('){
						pop(r1, op_stack);
						push(r1, rpn_stack);
						show_rpn(v);
					}
				}
			}
		}
	}

	//Move remaining operators from op_stack to rpn_stack
	for (string i : op_stack){
		pop(r1, op_stack);
		push(r1, rpn_stack);
	}

	//Create Reverse Polish notation string
	string rpn;
	for (string i : rpn_stack) rpn += i + " ";

	if (v) cout << "RPN: " << rpn << endl;

	for (int i = 0; i < rpn.length(); i++){

		c2 = rpn.at(i);
		
		if (num(c2)){

			//Get the current number and move the index i
			//to i + length of the number
			l0 = get_double(i, rpn);
			push(l0, out_stack);
			show_out(v);

		} else {		

			//Operators pop one or more operands from out_stack,
			//performs the operation, then it pushes back to
			//out_stack
			switch(c2){

				case '+': {
					pop(l1, out_stack); pop(l2, out_stack);
					l2 += l1;
					push(l2, out_stack); show_out(v); break;
				}

				case '-': {
					pop(l1, out_stack); pop(l2, out_stack); 
					l2 -= l1;
					push(l2, out_stack); show_out(v); break;
				}

				case '*': { 
					pop(l1, out_stack); pop(l2, out_stack);
					l2 *= l1;
					push(l2, out_stack); show_out(v); break;
				}

				case '/': {
					pop(l1, out_stack); pop(l2, out_stack);
					l2 /= l1;
					push(l2, out_stack); show_out(v); break;
				}

				case '^': {
					pop(l1, out_stack); pop(l2, out_stack);
					l2 = pow(l2, l1);
					push(l2, out_stack); show_out(v); break;
				}

				case '%': {
					pop(l1, out_stack); pop(l2, out_stack);
					l2 = fmod(l2, l1);
					push(l2, out_stack); show_out(v); break;
				}

				case 'k': {
					pop(l1, out_stack); l1 *= -1;
					push(l1, out_stack); show_out(v); break;
				}

				case 'a': {
					pop(l1, out_stack); l1 = fabs(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'b': {
					pop(l1, out_stack); l1 = log(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'c': {
					pop(l1, out_stack); l1 = sqrt(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'd': {
					pop(l1, out_stack); l1 = asin(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'f': {
					pop(l1, out_stack); l1 = acos(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'g': {
					pop(l1, out_stack); l1 = atan(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'h': {
					pop(l1, out_stack); l1 = sin(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'i': {
					pop(l1, out_stack); l1 = cos(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'j': {
					pop(l1, out_stack); l1 = tan(l1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'l': {
					pop(l1, out_stack); l1 = tgamma(l1+1);
					push(l1, out_stack); show_out(v); break;
				}

				case 'm': {
					pop(l1, out_stack); pop(l2, out_stack);
					l2 = log(l1)/log(l2);
					push(l2, out_stack); show_out(v); break;
				}
			}
		}
	}
	
	//Show the result with the highest precision allowed from double
	cout << "Result: ";
	cout << setprecision(numeric_limits <double> ::digits10 + 1);
	cout << out_stack.back() << endl;
}
