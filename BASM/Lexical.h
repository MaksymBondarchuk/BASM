#pragma once
#pragma warning(disable: 4018)

#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

using namespace std;

/*
Типи лексем:
0 - недопустима (лексична помилка)
1 - односимвольна
2 - бінарна константа
3 - директива
4 - команда
5 - ідентифікатор
6 - сегментний регістр
8 - 8-розрядний регістр
10 - десяткова константа
16 - шістнадцяткова константа
32 - 32-розрядний регістр
33 - 32-розрядний індексний регістр
*/


// Всі можливі роздільники
char sep[] = { ',', ' ', ':', '[', ']', '\t', '+', '-', '*', '/', ';', '(', ')' };
#define sep_len 13

// Всі допустимі односимвольні лексеми
char symb1[] = { ',', ':', '[', ']', '+', '-', '*', '/', '(', ')' };
#define symb1_len 10

// Всі допустимі директиви
string directive[] = { "END", "SEGMENT", "ENDS", "DB", "DW", "DD" };
#define directive_len 6

// Всі допустимі команди
string command[] = { "REP", "MOVSB", "MOVS", "ADD", "OR", "TEST", "MOV", "SHL", "JNZ" };
#define command_len 9

// Всі допустимі 32-розрядні регістри
string reg32[] = { "EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "ESP", "EBP" };
#define	reg32_len 8

// Всі допустимі 8-розрядні регістри
string reg8[] = { "AL", "AH", "BL", "BH", "CL", "CH", "DL", "DH" };
#define	reg8_len 8

// Всі допустимі сегментні регістри
string seg_reg[] = { "CS", "DS" };
#define	seg_reg_len 2

// Перевіряє, чи є <symb> роздільником
bool is_sep(char symb)
{
	for (int i = 0; i < sep_len; i++)
	if (sep[i] == symb)
		return 1;
	return 0;
}

// Перевіряє, чи є <symb> односимвольною лексемою
bool is_symb1(char symb)
{
	for (int i = 0; i < symb1_len; i++)
	if (symb1[i] == symb)
		return 1;
	return 0;
}

// Перевіряє, чи є <word> директивою
bool is_directive(string word)
{
	for (int i = 0; i < directive_len; i++)
	if (directive[i] == word)
		return 1;
	return 0;
}

// Перевіряє, чи є <word>  командою
bool is_command(string word)
{
	for (int i = 0; i < command_len; i++)
	if (command[i] == word)
		return 1;
	return 0;
}

// Перевіряє, чи є <word> ідентифікатором
bool is_idntf(string word)
{
	if (!word.size() || !isalpha(word[0]) || (word.size() > 8))
		return 0;

	for (int i = 0; i < word.size(); i++)
	if (!isalpha(word[i]) && !isdigit(word[i]))
		return 0;
	return 1;
}

// Перевіряє, чи є <word> бінарним літералом
bool is_bin_literal(string word)
{
	for (int i = 0; i < word.size() - 1; i++)
	if ((word[i] != '0') && (word[i] != '1'))
		return 0;
	if (word[word.size() - 1] != 'B')
		return 0;
	return 1;
}

// Перевіряє, чи є <word> десятковим літералом
bool is_dec_literal(string word)
{
	if (!isdigit(word[0]) && word[0] != '-')
		return 0;
	for (int i = 1; i < word.size() - 1; i++)
	if (!isdigit(word[i]))
		return 0;
	if ((word[word.size() - 1] != 'D') && !isdigit(word[word.size() - 1]))
		return 0;
	return 1;
}

// Перевіряє, чи є <word> шістнадцятковим літералом
bool is_hex_literal(string word)
{
	if (word.empty() || !isdigit(word[0]))
		return 0;
	for (int i = 1; i < word.size() - 1; i++)
	if (!isdigit(word[i]) && (word[i] != 'A') && (word[i] != 'B') && (word[i] != 'C') && (word[i] != 'D') && (word[i] != 'E') && (word[i] != 'F'))
		return 0;
	if (word[word.size() - 1] != 'H')
		return 0;
	return 1;
}

// Перевіряє, чи є <word> 32-розрядним регістром
bool is_reg32(string word)
{
	for (int i = 0; i < reg32_len; i++)
	if (reg32[i] == word)
		return 1;
	return 0;
}

// Перевіряє, чи є <word> 32-розрядним індексним регістром
bool is_index_reg(string word)
{
	for (int i = 0; i < reg32_len; i++)
	if ((reg32[i] == word) && (word != "ESP"))
		return 1;
	return 0;
}

// Перевіряє, чи є <word> 8-розрядним регістром
bool is_reg8(string word)
{
	for (int i = 0; i < reg8_len; i++)
	if (reg8[i] == word)
		return 1;
	return 0;
}

// Перевіряє, чи є <word> сегментним регістром
bool is_seg_reg(string word)
{
	for (int i = 0; i < seg_reg_len; i++)
	if (seg_reg[i] == word)
		return 1;
	return 0;
}

// Одна лексема
struct lexeme
{
	string name;	// Ім'я
	int type;		// Тип

	friend ostream& operator<<(ostream& stream, const lexeme &obj)
	{
		return stream << setw(12) << obj.name << setw(12) << obj.type << endl;
	}

	lexeme& operator=(const lexeme& obj)
	{
		name = obj.name;
		type = obj.type;
		return *this;
	}

	// Конструює лексему з її полів
	void set_data(string name, int type)
	{
		this->name = name;
		this->type = type;
	}
};

// Перевіряє таблицю лексем на помилки
bool is_lex_error(vector<lexeme> lex)
{
	for (unsigned short i = 0; i < lex.size(); i++)
	if (!lex[i].type)
		return 1;
	return 0;
}

// Розбиває <line> на лексеми
vector<lexeme> Lexical(string line)
{
	vector<lexeme> tbl;		// Те, що будемо повертати

	// Трансформуємо рядок у верхній регістр
	transform(line.begin(), line.end(), line.begin(), toupper);

	int i = 0, lex_nmb = 0;	// Індекс символа в реченні; номер лексеми
	bool was_semi = 0;
	while (i < line.size() && !was_semi && (line[i] != ';'))
	{
		string word;	// Ім'я лексеми

		while ((i < line.size()) && !is_sep(line[i]))	// Не кінець рядка, і не роздільник
		{
			word += line[i];
			i++;
		}

		// Якщо лексема - роздільник
		if (word.empty() && !is_symb1(line[i]))
		{
			i++;
			continue;
		}

		lex_nmb++;

		lexeme el;		// Лексема
		// Перевіряємо всі можливі варіанти
		if (is_bin_literal(word))
			el.set_data(word, 2);
		else
		if (is_dec_literal(word))
			el.set_data(word, 10);
		else
		if (is_hex_literal(word))
			el.set_data(word, 16);
		else
		if (is_directive(word))
			el.set_data(word, 3);
		else
		if (is_command(word))
			el.set_data(word, 4);
		else
		if (is_seg_reg(word))
			el.set_data(word, 6);
		else
		if (is_reg8(word))
			el.set_data(word, 8);
		else
		if (is_index_reg(word))
			el.set_data(word, 33);
		else
		if (is_reg32(word))
			el.set_data(word, 32);
		else
		if (is_idntf(word))
			el.set_data(word, 5);
		else
			el.set_data(word, 0);

		// Заносимо в таблицю
		bool was_insert_to_table = 0;
		if (!word.empty())
		{
			tbl.push_back(el);
			was_insert_to_table = 1;
		}

		// Якщо маємо односимвольну лексему.
		// Попередні пошуки її не виявлять
		if (is_symb1(line[i]))
		{
			string s;
			s += line[i];
			if (was_insert_to_table) lex_nmb++;
			el.set_data(s, 1);
			tbl.push_back(el);
		}

		if (line[i] == ';')
			was_semi = 1;

		i++;
	}

	// Знаходимо всі знаки "-"
	i = 1;
	while (i < tbl.size())
	{
		if (tbl[i].name == "-")
		{
			if (i && i + 1 < tbl.size() &&
				((tbl[i - 1].name == "DB") ||
				(tbl[i - 1].name == "DW") ||
				(tbl[i - 1].name == "DD") ||
				(tbl[i - 1].name == "+") ||
				(tbl[i - 1].name == "-") ||
				(tbl[i - 1].name == "*") ||
				(tbl[i - 1].name == "/") ||
				(tbl[i - 1].name == "(")) &&
				tbl[i + 1].type == 10)
			{
				tbl[i + 1].name = "-" + tbl[i + 1].name;
				for (int j = i; j < tbl.size() - 1; j++)
					tbl[j] = tbl[j + 1];
				tbl.pop_back();
			}
		}
		i++;
	}


	return tbl;
}