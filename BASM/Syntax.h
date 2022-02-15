#pragma once
#include "Lexical.h"

// Інвертує бінарний або шістнадцятковий літерал
string inverse(string s, unsigned short sys)
{
	string ret;
	if (sys == 2)	// Бінарний
	for (int i = 0; i < s.size(); i++)
	if (s[i] == '0')
		ret += "1";
	else
		ret += "0";
	if (sys == 16)	// Шістнадцятковий
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == 'F')
			ret += "0";
		if (s[i] == 'E')
			ret += "1";
		if (s[i] == 'D')
			ret += "2";
		if (s[i] == 'C')
			ret += "3";
		if (s[i] == 'B')
			ret += "4";
		if (s[i] == 'A')
			ret += "5";
		if (s[i] == '9')
			ret += "6";
		if (s[i] == '8')
			ret += "7";
		if (s[i] == '7')
			ret += "8";
		if (s[i] == '6')
			ret += "9";
		if (s[i] == '5')
			ret += "A";
		if (s[i] == '4')
			ret += "B";
		if (s[i] == '3')
			ret += "C";
		if (s[i] == '2')
			ret += "D";
		if (s[i] == '1')
			ret += "E";
		if (s[i] == '0')
			ret += "F";
	}
	return ret;
}

// Переводить літерал із системи числення sys до десяткової
long long sys_to_dec(string s, unsigned short sys = 10)
{
	long long res = 0;
	short sign = 1;		// Знак літерала
	for (int i = 0; i < s.size(); i++)
	{
		short x = 0;
		if (s[i] == '-') sign = -1;
		if (s[i] == '1') x = 1;
		if (s[i] == '2') x = 2;
		if (s[i] == '3') x = 3;
		if (s[i] == '4') x = 4;
		if (s[i] == '5') x = 5;
		if (s[i] == '6') x = 6;
		if (s[i] == '7') x = 7;
		if (s[i] == '8') x = 8;
		if (s[i] == '9') x = 9;
		if (s[i] == 'A') x = 10;
		if (s[i] == 'B') x = 11;
		if (s[i] == 'C') x = 12;
		if (s[i] == 'D') x = 13;
		if (s[i] == 'E') x = 14;
		if (s[i] == 'F') x = 15;

		res += (long long)pow(sys, s.size() - (i + 1)) * x;
	}

	return sign * res;
}

// Переводить літерал, записаний у рядку s у десяткову систему
long long num(string s)
{
	// Літерал - шістнадцятковий
	if (s[s.size() - 1] == 'H')
	{
		s.pop_back();
		if (s[0] == 'F' || s[0] == 'D' || s[0] == 'C' || s[0] == 'B' || s[0] == 'A' || s[0] == '9' || s[0] == '8')
			return -(sys_to_dec(inverse(s, 16), 16) + sys_to_dec("1", 2));
		return sys_to_dec(s, 16);
	}
	// Літерал - бінарний
	if (s[s.size() - 1] == 'B')
	{
		s.pop_back();
		if (s[0] == '1')
			return -(sys_to_dec(inverse(s, 2), 2) + sys_to_dec("1", 2));
		return sys_to_dec(s, 2);
	}
	// Літерал - десятковий
	if (s[s.size() - 1] == 'D')
	{
		s.pop_back();
		return sys_to_dec(s);
	}

	return sys_to_dec(s);
}

// Повертає пріоритет операції
short op_priority(string op)
{
	if (op == "(")
		return 0;
	if (op == "+" || op == "-")
		return 1;
	if (op == "*" || op == "/")
		return 2;
	return -1;
}

// Повертає значення абсолютного виразу (за алгоритмом Декстри)
int imm_nmb(vector<lexeme> lex, int start, int len)
{
	vector<double> nums;	// Стек літералів
	vector<string> ops;		// Стек операцій

	// Обхід абсолютного виразу
	for (int i = start; i < start + len; i++)
	{
		// Якщо літерал
		if (lex[i].type == 2 || lex[i].type == 10 || lex[i].type == 16)
			nums.push_back((double)num(lex[i].name));

		// Якщо операція
	again:
		if (lex[i].name == "+" || lex[i].name == "-" || lex[i].name == "*" || lex[i].name == "/")
		{
			if (ops.empty() || op_priority(ops[ops.size() - 1]) < op_priority(lex[i].name))
				ops.push_back(lex[i].name);
			else
			{
				int y = nums[nums.size() - 1];
				nums.pop_back();
				int x = 0;
				if (!nums.empty())
				{
					x = nums[nums.size() - 1];
					nums.pop_back();
				}
				string op = ops[ops.size() - 1];
				ops.pop_back();
				if (op == "+")
					x += y;
				if (op == "-")
					x -= y;
				if (op == "*")
					x *= y;
				if (op == "/")
					x /= y;
				nums.push_back(x);
				goto again;
			}
		}

		// Якщо відкриваюча дужка
		if (lex[i].name == "(")
			ops.push_back(lex[i].name);

		// Якщо закриваюча дужка
		if (lex[i].name == ")")
		{
			while (ops[ops.size() - 1] != "(")
			{
				string op = ops[ops.size() - 1];
				ops.pop_back();
				int y = nums[nums.size() - 1];
				nums.pop_back();
				int x = 0;
				if (!nums.empty())
				{
					x = nums[nums.size() - 1];
					nums.pop_back();
				}
				if (op == "+")
					x += y;
				if (op == "-")
					x -= y;
				if (op == "*")
					x *= y;
				if (op == "/")
					x /= y;
				nums.push_back(x);
			}
			ops.pop_back();
		}
	}

	// Якщо залишились операції
	while (!ops.empty())
	{
		string op = ops[ops.size() - 1];
		ops.pop_back();
		double y = nums[nums.size() - 1];
		nums.pop_back();
		double x = 0;
		if (!nums.empty())
		{
			x = nums[nums.size() - 1];
			nums.pop_back();
		}
		if (op == "+")
			x += y;
		if (op == "-")
			x -= y;
		if (op == "*")
			x *= y;
		if (op == "/")
			x /= y;
		nums.push_back(x);
	}

	return (int)nums[0];
	//return 1;
}

// Повертає розмір (у байтах) абсолютного виразу
int imm_size(vector<lexeme> lex, int start, int len)
{
	long long x = imm_nmb(lex, start, len);
	if (-128 <= x && x <= 127)
		return 1;
	if (-2147483648ll <= x && x <= 2147483647ll)
		return 4;
	return 0;
}

// Таблиця структури речення
struct ln_struct
{
	int label;
	vector<int> mnem;
	vector<int> op1;
	vector<int> op2;
	bool err;
	bool empty;
	int just;

	ln_struct()
	{
		empty = 1;
		err = 0;
		label = -1;
		just = 0;
	}

	friend ostream& operator<<(ostream& stream, const ln_struct &obj)
	{
		if (obj.err)
			stream << "Syntax error" << endl;
		//else
		//{
		//stream << "ln struct" << endl;
		//stream << setw(4) << obj.empty << " | ";
		stream << setw(4) << obj.label << " | ";
		/*for (int i = 0; i < obj.mnem.size(); i++)
		stream << setw(4) << obj.mnem[i];*/
		if (obj.mnem.size())
			stream << setw(4) << obj.mnem[0] << setw(4) << obj.mnem.size();

		stream << " | ";
		if (obj.op1.size())
			stream << setw(4) << obj.op1[0] << setw(4) << obj.op1.size();

		stream << " | ";
		if (obj.op2.size())
			stream << setw(4) << obj.op2[0] << setw(4) << obj.op2.size();
		stream << endl;

		//stream << "empty: " << obj.empty << "     err: " << obj.err << endl;
		//}

		return stream;
	}
};

// Перевіряє, чи може лексема nxt бути після cur в абсолютному виразі
bool correct(string cur, string nxt)
{
	if (cur == "(")
	if (is_bin_literal(nxt) || is_dec_literal(nxt) || is_hex_literal(nxt) || (nxt == "("))
		return 1;

	if (cur == ")")
	if ((nxt == "+") || (nxt == "-") || (nxt == "*") || (nxt == "/") || (nxt == ")"))
		return 1;

	if (is_bin_literal(cur) || is_dec_literal(cur) || is_hex_literal(cur))
	if ((nxt == "(") || (nxt == ")") || (nxt == "+") || (nxt == "-") || (nxt == "*") || (nxt == "/"))
		return 1;

	if ((cur == "+") || (cur == "-") || (cur == "*") || (cur == "/"))
	if (is_bin_literal(nxt) || is_dec_literal(nxt) || is_hex_literal(nxt) || (nxt == "("))
		return 1;

	return 0;
}

// Перевіряє, чи є частина структури речення від start до len абсолютним виразом
bool is_imm(vector<lexeme> lex, int start, int len)
{
	// Коректність початку
	short opn = 0, cls = 0;
	if (lex[start].name != "-" && lex[start].name != "(" && !is_bin_literal(lex[start].name) && !is_dec_literal(lex[start].name) && !is_hex_literal(lex[start].name))
		return 0;
	// Коректність всього іншого
	for (int i = start; i < start + len - 1; i++)
	{
		if (!correct(lex[i].name, lex[i + 1].name)) return 0;
		if (lex[i].name == "(") opn++;
		if (lex[i].name == ")") cls++;
	}
	// Коректність кінця
	if (lex[start + len - 1].name != ")" && !is_bin_literal(lex[start + len - 1].name) && !is_dec_literal(lex[start + len - 1].name) && !is_hex_literal(lex[start + len - 1].name))
		return 0;
	// Коректна кількість дужок
	if (lex[start + len - 1].name == ")") cls++;
	if (cls - opn) return 0;
	
	return 1;
}

// Перевіряє, чи є лексема з номером start структури речення регістром
bool is_reg(vector<lexeme> lex, int start)
{
	if ((lex[start].type != 8) && (lex[start].type != 32) && (lex[start].type != 33))
		return 0;
	return 1;
}

// Перевіряє, чи є лексема з номером start структури речення ідентифікатором
bool is_ident(vector<lexeme> lex, int start)
{
	if (lex[start].type == 5)
		return 1;
	return 0;
}

// Перевіряє, чи є частина структури речення від start до len операндом пам'яті
bool is_mem(vector<lexeme> lex, int start, int len)
{
	if (len < 4)
		return 0;
	if (lex[start].type == 5) // <ident><[><index_reg><]>
	if (lex[start + 1].name == "[")
	if (lex[start + 2].type == 33)
	if (lex[start + 3].name == "]")
		return 1;

	if (len < 6)
		return 0;
	if (lex[start].type == 6) // <segreg><:><ident><[><index_reg><]>
	if (lex[start + 1].name == ":")
	if (lex[start + 2].type == 5)
	if (lex[start + 3].name == "[")
	if (lex[start + 4].type == 33)
	if (lex[start + 5].name == "]")
		return 1;

	return 0;
}

// Перевіряє, чи є рядок директивою SEGMENT
bool is_SEGMENT(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.label != -1) && (ln.mnem.size() == 1))
	if ((lex[ln.label].type == 5) && (lex[ln.mnem[0]].name == "SEGMENT"))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок директивою ENDS
bool is_ENDS(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.label != -1) && (ln.mnem.size() == 1))
	if ((lex[ln.label].type == 5) && (lex[ln.mnem[0]].name == "ENDS"))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок директивою END
bool is_END(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.label == -1) && (ln.mnem.size() == 1) && (ln.op1.size() == 1))
	if ((lex[ln.mnem[0]].name == "END") && (lex[ln.op1[0]].type == 5))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок директивою DB
bool is_DB(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.label != -1) && (ln.mnem.size() == 1) && ln.op1.size())
	if ((lex[ln.label].type == 5) && (lex[ln.mnem[0]].name == "DB") && is_imm(lex, ln.op1[0], ln.op1.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок директивою DW
bool is_DW(ln_struct ln, vector<lexeme> lex)
{
	if (ln.label != -1 && (ln.mnem.size() == 1) && ln.op1.size())
	if ((lex[ln.label].type == 5) && (lex[ln.mnem[0]].name == "DW") && is_imm(lex, ln.op1[0], ln.op1.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок директивою DD
bool is_DD(ln_struct ln, vector<lexeme> lex)
{
	if (ln.label != -1 && (ln.mnem.size() == 1) && ln.op1.size())
	if ((lex[ln.label].type == 5) && (lex[ln.mnem[0]].name == "DD") && is_imm(lex, ln.op1[0], ln.op1.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою MOVSB
bool is_MOVSB(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && !ln.op1.size() && !ln.op2.size())
	if (ln.label && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if (lex[ln.mnem[0]].name == "MOVSB")
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою REP MOVSB
bool is_REP_MOVS(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 2) && ln.op1.size() && ln.op2.size())
	if ((ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":")) || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "REP") && (lex[ln.mnem[1]].name == "MOVS") && is_mem(lex, ln.op1[0], ln.op1.size()) && is_mem(lex, ln.op2[0], ln.op2.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою ADD
bool is_ADD(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && (ln.op1.size() == 1) && (ln.op2.size() == 1))
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "ADD") && is_reg(lex, ln.op1[0]) && is_reg(lex, ln.op2[0]))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою OR
bool is_OR(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && (ln.op1.size() == 1) && ln.op2.size())
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "OR") && is_reg(lex, ln.op1[0]) && is_mem(lex, ln.op2[0], ln.op2.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою TEST
bool is_TEST(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && ln.op1.size() && (ln.op2.size() == 1))
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "TEST") && is_mem(lex, ln.op1[0], ln.op1.size()) && is_reg(lex, ln.op2[0]))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою MOV
bool is_MOV(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && (ln.op1.size() == 1) && ln.op2.size())
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "MOV") && is_reg(lex, ln.op1[0]) && is_imm(lex, ln.op2[0], ln.op2.size()))
	{
		//cout << imm_size(lex, ln.op2[0] - 1, ln.op2.size()) << endl;
		return 1;
	}
	return 0;
}

// Перевіряє, чи є рядок командою SHL
bool is_SHL(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && ln.op1.size() && ln.op2.size())
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "SHL") && is_mem(lex, ln.op1[0], ln.op1.size()) && is_imm(lex, ln.op2[0], ln.op2.size()))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок командою JNZ
bool is_JNZ(ln_struct ln, vector<lexeme> lex)
{
	if ((ln.mnem.size() == 1) && (ln.op1.size() == 1) && !ln.op2.size())
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":") || (ln.label == -1))
	if ((lex[ln.mnem[0]].name == "JNZ") && (lex[ln.op1[0]].type == 5))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок міткою (тільки мітка)
bool is_LABEL(ln_struct ln, vector<lexeme> lex)
{
	if (!ln.mnem.size() && !ln.op1.size() && !ln.op2.size())
	if (ln.label != -1 && (lex[0].type == 5) && (lex[1].name == ":"))
		return 1;
	return 0;
}

// Перевіряє, чи є рядок пустим
bool is_EMPTY(ln_struct ln, vector<lexeme> lex)
{
	if (ln.label == -1 && !ln.mnem.size() && !ln.op1.size() && !ln.op2.size())
		return 1;
	return 0;
}

// Повертає новий ідентифікатор, визначений у рядку (або повертає пустий елемент типу string)
string get_new_ident(ln_struct ln, vector<lexeme> lex)
{
	if (is_SEGMENT(ln, lex) || is_ENDS(ln, lex) || is_DB(ln, lex) || is_DW(ln, lex) || is_DD(ln, lex) || is_LABEL(ln, lex))
		return lex[0].name;

	if (is_END(ln, lex))
		return lex[ln.op1[0]].name;

	if (is_REP_MOVS(ln, lex) || is_ADD(ln, lex) || is_OR(ln, lex) || is_TEST(ln, lex) || is_MOV(ln, lex) || is_SHL(ln, lex) || is_JNZ(ln, lex))
	if (ln.label != -1)
		return lex[0].name;

	string empty;
	return empty;
}

// Будує по татаблиці лексем таблицю структури речення
ln_struct Syntax(vector<lexeme> lex)
{
	ln_struct ln_empty;
	ln_struct ln_str;

	// Рядок пустий (рядок з коментарем вважається пустим)
	if (lex.empty())
		return ln_empty;

	ln_str.empty = 0;

	// Поле мітки/імені
	int i = 0;
	if (2 <= lex.size())
	{
		if (lex[1].name == ":")
		{
			ln_str.label = 0;
			i = 2;
		}
		else
		if (lex[1].type == 3 && lex[0].type != 4)
		{
			ln_str.label = 0;
			i = 1;
		}
	}

	if (i < lex.size())
	{
		// Поле мнемокоду
		ln_str.mnem.push_back(i);
		i++;

		if ((i < lex.size()) && ((lex[i].type == 3) || (lex[i].type == 4)))
		{
			ln_str.mnem.push_back(i);
			i++;
		}

		// Поле 1го операнда
		for (; (i < lex.size()) && (lex[i].name != ","); i++)
			ln_str.op1.push_back(i);

		i++;
		// Поле 2го оперенда
		for (; i < lex.size(); i++)
			ln_str.op2.push_back(i);
	}

	return ln_str;
}