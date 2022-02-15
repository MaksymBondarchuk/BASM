#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

// Файли компілятора
struct Files
{
	string source;		// Вхідного коду
	string lst_name;	// Лістингу
	bool is_source;		// Ознака коректності файлів
	bool is_listing;

};

Files get_files()
{
	// повідомлення, аналогічні MASM-івським
	cout << "Bondarchuk (R) Macro Assembler" << endl;
	cout << "Copyright (C) Bondarchuk Maxim. All rights reserved." << endl;

	Files ret;		// Те, що будемо повертати
	string sret;	// Ім'я файлу вхідного коду
	while (1)
	{
		// Отримуємо ім'я файлу вхідного коду
		cout  << endl << "Source filename [.ASM]: ";
		string source_name;
		getline(cin, source_name, '\n');

		// Видаляємо пропуски
		string without_space;
		for (unsigned int i = 0; i < source_name.size(); i++)
			if (source_name[i] !=  ' ')
				without_space += source_name[i];

		// Якщо вийшов пустий рядок
		if (without_space.empty())
		{
			cout << "Line invalid, start again";
			continue;
		}

		// Дивимось, чи було вказане розширення файлу
		size_t pos = without_space.find(".");
		if (pos == string::npos)
			without_space += ".ASM";

		sret = without_space;
		break;
	}

	// Запитуємо ім'я файлу лістингу
	cout << "Source listing  [NUL.LST]: ";
	string lret;
	getline(cin, lret, '\n');	// Те, що ввів користувач
	// Трансформуємо рядок у верхній регістр
	transform(lret.begin(), lret.end(), lret.begin(), toupper);
	string without_space;
	// Видаляємо пропуски
	for (unsigned int i = 0; i < lret.size(); i++)
		if (lret[i] !=  ' ')
			without_space += lret[i];
	if (without_space.empty())
		ret.is_listing = 0;

	if (ret.is_listing)
	{
		// Перевіряємо, чи було вказане розширення файлу
		size_t pos = without_space.find(".");
		if (pos == string::npos)
			without_space += ".LST";
	}
	ret.is_listing = 1;
	ret.lst_name = without_space;

	// Перевіряємо існування файлу вхідного коду
	ifstream f(sret);
	if (!f.is_open())	// Якщо такого немає
	{
		cout << "Unable to open input file: " << sret << endl;
		ret.is_source = 0;
	}
	else ret.is_source = 1;	// Все добре
	f.close();

	// Заносимо імена файлів
	ret.source = sret;


	return ret;
}