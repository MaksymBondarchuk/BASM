#pragma once
#pragma warning (disable: 4996)

#include "First.h"
#include <ctime>

void Second(Files fls, Tables& t)
{
	ofstream g(fls.lst_name);
	g << "Bondarchuk (R) Macro Assembler";		// Виводимо автора продукту
	// Визначаємо час початку виведення лістингу
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	g << setw(40) << asctime(timeinfo) << endl;

	ifstream f(fls.source);
	int ln_nmb = 0;				// Номар рядка вхідного коду
	short active_seg = -1;		// Номер активного сегменту
	int offset = 0;				// Поточне зміщення в сегменті
	bool not_end = 1;			// Ознака відсутності директиви END

	while (!f.eof())
	{
		string line;
		getline(f, line, '\n');
		ln_nmb++;

		// Отримуємо таблиці по рядку
		vector<lexeme> lex = Lexical(line);
		ln_struct ln = Syntax(lex);
		m_code code;

		// Якщо лексична помилка
		if (t.is_err(ln_nmb))
			_asm
		{
			jmp err2;
		}

		// Якщо відкриття нового сегменту (для визначення активного сегменту)
		if (is_SEGMENT(ln, lex) && (active_seg == -1))
		{
			string seg = get_new_ident(ln, lex);

			if (seg == t.DS.name)
				active_seg = 0;
			if (seg == t.CS.name)
				active_seg = 1;

			offset = 0;
		}

		// Якщо закриття сегменту
		if (is_ENDS(ln, lex) && (active_seg != -1))
			active_seg = -1;

		// Якщо є стрибок на неіснуючу мітку
		if (is_JNZ(ln, lex))
		if (!is_in_table(lex[ln.op1[0]].name, t))
		{
			for (unsigned int i = 0; i < t.ids.size(); i++)
			if (offset < t.ids[i].offset)
				t.ids[i].offset -= 6;
			else
			if (offset == t.ids[i].offset)
				t.ids[i].name.erase();
			if (!active_seg)
				t.DS.size -= 6;
			if (active_seg)
				t.CS.size -= 6;
			t.err_lns.push_back(ln_nmb);
			_asm
			{
				jmp err2;
			}
		}

		code = machine_code(ln, lex, t, active_seg, offset);	// Отримуємо машинний код
		if (!lex.empty() && !is_END(ln, lex))
		{
			// Виводимо зміщення
			g << hex << uppercase << setw(4) << setfill('0') << offset << "  ";

			// Префікс REP
			if (!code.rep.empty())
				g << hex << uppercase << setw(2) << setfill('0') << code.rep[0] << "/ ";

			// Префікс зміни сегменту
			if (!code.prefix.empty())
				g << hex << uppercase << setw(2) << setfill('0') << code.prefix[0] << ": ";

			// Перший байт коду команди
			if (1 <= code.machine_B.size())
				g << hex << uppercase << setw(2) << setfill('0') << code.machine_B[0] << " ";

			// Другий байт коду команди 
			if (2 == code.machine_B.size())
			{
				char buf[10];
				sprintf(buf, "%x", code.machine_B[1]);
				string s = buf;
				string out;
				for (unsigned short j = s.size() - 2; j < s.size(); j++)
					out.push_back(s[j]);
				transform(out.begin(), out.end(), out.begin(), toupper);
				g << hex << uppercase << setw(2) << setfill('0') << out << " ";
			}

			// Зміщення операнду пам'яті
			if (!code.offset.empty())
			if (code.offset[0] != -1)
				g << hex << uppercase << setw(8) << setfill('0') << code.offset[0] << " ";

			// Якщо формат команди: command mem, imm
			if (!code.offset.empty() && (!code.value8.empty() || !code.value16.empty() || !code.value32.empty()))
				g << "R ";

			// Значення однобайтного абсолютного виразу
			if (!code.value8.empty())
			{
				char buf[10];
				sprintf(buf, "%x", code.value8[0]);
				string s = buf;
				s = "00" + s;
				string out;
				for (unsigned short j = s.size() - 2; j < s.size(); j++)
					out.push_back(s[j]);
				transform(out.begin(), out.end(), out.begin(), toupper);
				g << hex << uppercase << setw(2) << setfill('0') << out << " ";
			}

			// Значення двобайтного абсолютного виразу
			if (!code.value16.empty())
				g << hex << uppercase << setw(4) << setfill('0') << (short)code.value16[0] << " ";

			// Значення чотирьохбайтного абсолютного виразу
			if (!code.value32.empty())
				g << hex << uppercase << setw(8) << setfill('0') << code.value32[0] << " ";

			g << "    " << line << endl;
		}
		else
			g << "      " << line << endl;

		// Додаємо до зміщення розмір команди
		offset += code.size();

		// Якщо зустріли директиву END
		if (is_END(ln, lex))
		{
			if (active_seg == 0)
				t.DS.size = offset;
			if (active_seg == 1)
				t.CS.size = offset;

			not_end = 0;
			break;
		}
		continue;

		// Виводимо інформацію про помилку (у файл і консоль)
		_asm
		{
		err2:
		}
		g << "      " << line << endl;

		g << fls.source << "(" << dec << ln_nmb << "): " << "error" << endl;
		cout << fls.source << "(" << dec << ln_nmb << "): " << "error" << endl;
	}

	// Якщо не було директиви END
	if (not_end)
	{
		g << fls.source << "(" << dec << ln_nmb << "): " << "error: no END directive" << endl;
		cout << fls.source << "(" << dec << ln_nmb << "): " << "error: no END directive" << endl;
	}



	f.close();
	g.close();
}