#pragma once

#include "Syntax.h"
#include "get_files.h"

using namespace std;

// Ідентифікатор у таблиці ідентифікаторів
struct ident
{
	string name;
	string seg_name;
	int offset;
	short type;
};

// Ідентифікатор сегменту у таблиці сегментів
struct seg
{
	string name;
	int size;
};

// Тааблиці, що генеруються на першому проході
struct Tables
{
	vector<ident> ids;		// Таблиця ідентифікаторів
	vector<string> segs;	// Таблиця сегментів
	seg DS, CS;				// Сегментні регістри (даних, кодів)
	vector<int> err_lns;	// Вектор номерів рядків, у яких виявлено помилку

	friend ostream& operator<<(ostream& stream, const Tables &obj)
	{
		stream << setw(12) << "Name" << setw(12) << "Segment" << setw(12) << "Offset" << setw(12) << "Type" << endl;
		for (int i = 0; i < obj.ids.size(); i++)
		if (!obj.ids[i].name.empty())
			stream << setw(12) << obj.ids[i].name << setw(12) << obj.ids[i].seg_name << setw(8) << " " << hex << uppercase << setw(4) << setfill('0') << obj.ids[i].offset << dec << setfill(' ') << setw(12) << obj.ids[i].type << endl;

		stream << endl;
		if (obj.CS.name != "")
			stream << setw(12) << "Name" << setw(12) << "Size" << setw(12) << "Log seg" << endl;
		if (obj.DS.name != "")
			stream << setw(12) << obj.DS.name << "        " << hex << uppercase << setw(4) << setfill('0') << obj.DS.size << dec << setfill(' ') << setw(12) << "'Data'" << endl;
		stream << setw(12) << obj.CS.name << "        " << hex << uppercase << setw(4) << setfill('0') << obj.CS.size << dec << setfill(' ') << setw(12) << "'Code'" << endl;

		return stream;
	}

	// Перевіряє, чи є повилка у рядку з номером nmb
	bool is_err(int nmb)
	{
		for (int i = 0; i < err_lns.size(); i++)
		if (err_lns[i] == nmb)
			return 1;
		return 0;
	};
};

// Коди регістрів (№ у векторі) (можуть бути ідентичними з лексичним)
string regs8[] = { "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH" };
string regs32[] = { "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI" };
string index_regs[] = { "EAX", "ECX", "EDX", "EBX", "fict", "EBP", "ESI", "EDI" };

// Повертає перший операнд рядка
string get_op1(ln_struct ln, vector<lexeme> lex)
{
	if ((3 <= ln.op1.size()) && (lex[ln.op1[1]].name == ":"))
		return lex[ln.op1[2]].name;
	return lex[ln.op1[0]].name;
}

// Повертає другий операнд рядка
string get_op2(ln_struct ln, vector<lexeme> lex)
{
	if ((3 <= ln.op2.size()) && (lex[ln.op2[1]].name == ":"))
		return lex[ln.op2[2]].name;
	return lex[ln.op2[0]].name;
}

// Повертає розмір іденнтифікатора (з таблиці)
short get_id_size(string id, Tables t)
{
	for (int i = 0; i < t.ids.size(); i++)
	if (t.ids[i].name == id)
		return t.ids[i].type;
	return 0;
}

// Повертає розмір регістра (в даному проекті всі розміри - у байтах)
short get_reg_size(string reg)
{
	if (is_reg32(reg))
		return 4;
	if (is_reg8(reg))
		return 1;
	return -7;
}

// Перевіряє, чи є ідентифікатор word у таблиці t
short is_in_table(string word, Tables t)
{
	for (int i = 0; i < t.ids.size(); i++)
	if (t.ids[i].name == word)
		return 1;
	return 0;
}

// Перевіряє правильність розрядності даних
bool ops_bit(ln_struct ln, vector<lexeme> lex, Tables t)
{
	// Перевіо=рємо всі можливі директиви
	if (is_DB(ln, lex))
	if (imm_size(lex, ln.op1[0], ln.op1.size()) == 1)
		return 1;

	if (is_DW(ln, lex))
	if (-32768 <= imm_nmb(lex, ln.op1[0], ln.op1.size()) && imm_nmb(lex, ln.op1[0], ln.op1.size()) <= 32767)
		return 1;

	if (is_DD(ln, lex))
	if (imm_size(lex, ln.op1[0], ln.op1.size()) == 4 || imm_size(lex, ln.op1[0], ln.op1.size()) == 1)
		return 1;

	// Перевіо=рємо всі можливі команди
	if (is_MOVSB(ln, lex))
		return 1;

	if (is_REP_MOVS(ln, lex))
	if (get_id_size(get_op1(ln, lex), t) == get_id_size(get_op2(ln, lex), t) && get_id_size(get_op1(ln, lex), t) != 2)
		return 1;

	if (is_ADD(ln, lex))
	if (get_reg_size(get_op1(ln, lex)) == get_reg_size(get_op2(ln, lex)))
		return 1;

	if (is_OR(ln, lex))
	if (get_reg_size(get_op1(ln, lex)) == get_id_size(get_op2(ln, lex), t))
		return 1;

	if (is_TEST(ln, lex))
	if (get_id_size(get_op1(ln, lex), t) == get_reg_size(get_op2(ln, lex)))
		return 1;

	if (is_MOV(ln, lex))
	if (get_reg_size(get_op1(ln, lex)) == imm_size(lex, ln.op2[0], ln.op2.size()) || imm_size(lex, ln.op2[0], ln.op2.size()) == 1)
		return 1;

	if (is_SHL(ln, lex))
	if (((get_id_size(get_op1(ln, lex), t) == 4) || (get_id_size(get_op1(ln, lex), t) == 1)) && (imm_nmb(lex, ln.op2[0], ln.op2.size()) <= 31))
		return 1;

	if (is_JNZ(ln, lex))
		return 1;

	// Або якщо мітка
	if (is_LABEL(ln, lex))
		return 1;

	return 0;
}

// Машинний код команди
struct m_code
{
	vector<int> rep;
	vector<int> prefix;
	vector<int> machine_B;
	vector<int> offset;
	vector<int> value8;
	vector<int> value16;
	vector<int> value32;

	m_code& operator=(const m_code& obj)
	{
		rep.erase(rep.begin(), rep.end());
		prefix.erase(prefix.begin(), prefix.end());
		machine_B.erase(machine_B.begin(), machine_B.end());
		offset.erase(offset.begin(), offset.end());
		value8.erase(value8.begin(), value8.end());
		value16.erase(value16.begin(), value16.end());
		value32.erase(value32.begin(), value32.end());
		for (unsigned short i = 0; i < obj.rep.size(); i++)
			rep.push_back(obj.rep[i]);
		for (unsigned short i = 0; i < obj.prefix.size(); i++)
			prefix.push_back(obj.prefix[i]);
		for (unsigned short i = 0; i < obj.machine_B.size(); i++)
			machine_B.push_back(obj.machine_B[i]);
		for (unsigned short i = 0; i < obj.offset.size(); i++)
			offset.push_back(obj.offset[i]);
		for (unsigned short i = 0; i < obj.value8.size(); i++)
			value8.push_back(obj.value8[i]);
		for (unsigned short i = 0; i < obj.value16.size(); i++)
			value16.push_back(obj.value16[i]);
		for (unsigned short i = 0; i < obj.value32.size(); i++)
			value32.push_back(obj.value32[i]);
		return *this;
	}

	// Розмір команди
	int size() const
	{
		return rep.size() + prefix.size() + machine_B.size() + 4 * offset.size() + value8.size() + 2 * value16.size() + 4 * value32.size();
	}
};

// Повертає код регістра
short get_reg_code(string name)
{
	for (int i = 0; i < 8; i++)
	if ((regs8[i] == name) || (regs32[i] == name))
		return i;
	return -1;
}

// Повертає біт w
short get_w(string name, Tables t)
{
	for (int i = 0; i < 8; i++)
	if (regs8[i] == name)
		return 0;
	for (int i = 0; i < 8; i++)
	if (regs32[i] == name)
		return 1;
	if (get_id_size(name, t) == 4)
		return 1;
	if (get_id_size(name, t) == 1)
		return 0;
	return -1;
}

// Повертає доповнення біту c
short get_not_c(int imm)
{
	if (imm == 1)
		return 1;
	return 0;
}

// Повертає код індексного регістра
short get_index_reg_code(string name)
{
	for (int i = 0; i < 8; i++)
	if (index_regs[i] == name)
		return i;
	return -1;
}

// Поветрає зміщення ідентифікатора (з таблиці t)
int get_id_offset(string id, Tables t)
{
	for (int i = 0; i < t.ids.size(); i++)
	if (t.ids[i].name == id)
		return t.ids[i].offset;
	return -1;
}

// Встановлює зміщення ідентифікатора (в таблиці t)
void set_id_offset(string id, Tables& t, int off)
{
	for (int i = 0; i < t.ids.size(); i++)
	if (t.ids[i].name == id)
	{
		t.ids[i].offset = off;
		return;
	}
}

// Повертає номер сегменту ідентифікатора id
int get_id_seg(string id, Tables& t)
{
	for (int i = 0; i < t.ids.size(); i++)
	if (t.ids[i].name == id)
		return t.ids[i].seg_name == t.CS.name;
	return -1;
}

// Повертає машинний код команди або директиви
m_code machine_code(ln_struct ln, vector<lexeme> lex, Tables t, short active_seg, int offset)
{
	m_code ret;

	// Перебираємо всі директиви
	if (is_DB(ln, lex))
		ret.value8.push_back(imm_nmb(lex, ln.op1[0], ln.op1.size()));

	if (is_DW(ln, lex))
		ret.value16.push_back(imm_nmb(lex, ln.op1[0], ln.op1.size()));

	if (is_DD(ln, lex))
		ret.value32.push_back(imm_nmb(lex, ln.op1[0], ln.op1.size()));

	// Та всі команди
	if (is_MOVSB(ln, lex))		// 1010 0100
		ret.machine_B.push_back(0xA4);

	if (is_REP_MOVS(ln, lex))	// 1111 0011 (REP) | 1010 0100
	{
		string id1 = lex[ln.op1[ln.op1.size() - 4]].name;
		string id2 = lex[ln.op2[ln.op2.size() - 4]].name;

		ret.rep.push_back(243);

		if (get_id_seg(id1, t) || get_id_seg(id2, t))
			ret.prefix.push_back(0x2E);

		ret.machine_B.push_back(0xA4);
	}

	if (is_ADD(ln, lex))		// 0000 00dw | mod reg r/m
	{
		string reg1 = lex[ln.op1[0]].name;
		string reg2 = lex[ln.op2[0]].name;

		int machine_B = 0x2 + get_w(reg1, t);
		ret.machine_B.push_back(machine_B);

		int ops_B = 0xC0 + (get_reg_code(reg1) << 3) + get_reg_code(reg2);
		ret.machine_B.push_back(ops_B);
	}

	if (is_OR(ln, lex))			// 0000 10dw | mod reg r/m
	{
		string reg1 = lex[ln.op1[0]].name;
		string index_reg = lex[ln.op2[ln.op2.size() - 2]].name;

		if (get_id_seg(lex[ln.op2[ln.op2.size() - 4]].name, t))
			ret.prefix.push_back(0x2E);

		if (index_reg == "EBP" && ret.prefix.empty())
			ret.prefix.push_back(0x3E);

		int machine_B = 0xA + get_w(reg1, t);
		ret.machine_B.push_back(machine_B);

		int ops_B = 0x80 + (get_reg_code(reg1) << 3) + get_index_reg_code(index_reg);
		ret.machine_B.push_back(ops_B);

		ret.offset.push_back(get_id_offset(lex[ln.op2[ln.op2.size() - 4]].name, t));
	}

	if (is_TEST(ln, lex))		// 1000 010w | mod reg r/m
	{
		string reg2 = lex[ln.op2[0]].name;
		string index_reg = lex[ln.op1[ln.op1.size() - 2]].name;

		if (get_id_seg(lex[ln.op1[ln.op1.size() - 4]].name, t))
			ret.prefix.push_back(0x2E);

		if (index_reg == "EBP" && ret.prefix.empty())
			ret.prefix.push_back(0x3E);

		int machine_B = 0x84 + get_w(reg2, t);
		ret.machine_B.push_back(machine_B);

		int ops_B = 0x80 + (get_reg_code(reg2) << 3) + get_index_reg_code(index_reg);
		ret.machine_B.push_back(ops_B);

		ret.offset.push_back(get_id_offset(lex[ln.op1[ln.op1.size() - 4]].name, t));
	}

	if (is_MOV(ln, lex))		// 1011 w reg | data [data], w = 1 якщо reg32
	{
		string reg1 = lex[ln.op1[0]].name;

		int machine_B = 0xB0 + (get_w(reg1, t) << 3) + get_reg_code(reg1);
		ret.machine_B.push_back(machine_B);

		if (get_w(reg1, t))
			ret.value32.push_back(imm_nmb(lex, ln.op2[0], ln.op2.size()));
		else
			ret.value8.push_back(imm_nmb(lex, ln.op2[0], ln.op2.size()));
	}

	if (is_SHL(ln, lex))		// 110 not c 000w | mod 100 r/m
	{
		string index_reg = lex[ln.op1[ln.op1.size() - 2]].name;
		string id = lex[ln.op1[ln.op1.size() - 4]].name;
		int imm = imm_nmb(lex, ln.op2[0], ln.op2.size());

		if (get_id_seg(id, t))
			ret.prefix.push_back(0x2E);

		if (index_reg == "EBP" && ret.prefix.empty())
			ret.prefix.push_back(0x3E);

		int machine_B = 0xC0 + (get_not_c(imm) << 4) + get_w(id, t);
		ret.machine_B.push_back(machine_B);

		int ops_B = 0xA0 + get_index_reg_code(index_reg);
		ret.machine_B.push_back(ops_B);

		ret.offset.push_back(get_id_offset(lex[ln.op1[ln.op1.size() - 4]].name, t));

		if (imm != 1)
			ret.value8.push_back(imm);
	}

	if (is_JNZ(ln, lex))		// dist 0101, де dist = 0111 - short, dist = 1000 - near
	{
		string id = lex[ln.op1[0]].name;
		int off = get_id_offset(id, t);

		if (off <= offset && off != -1)	// Назад
		{
			int x = 0x85;
			if (-128 <= off - offset && off - offset <= 127)
				x = 0x75;
			ret.machine_B.push_back(x);
			ret.machine_B.push_back(-(offset + 2 - off));
		}
		else // Вперед
		{
			ret.machine_B.push_back(0x0F);
			int x = 0x85;
			if (-128 <= off - offset && off - offset <= 127)
				x = 0x75;
			ret.machine_B.push_back(x);
			ret.offset.push_back(off);
		}
	}

	return ret;
}

// Перший прохід
Tables First(Files fls)
{
	ofstream g(fls.lst_name);
	ifstream f(fls.source);
	int ln_nmb = 0;			// Номер рядка у файлі
	short active_seg = -1;	// Активний сегмент (відповідно до методички)
	Tables t;				// Таблиці
	int offset = 0;			// Поточне зміщення
	vector<int> ins;		// Номери рядків команд в сегменті даних

	while (!f.eof())
	{
		string line;
		getline(f, line, '\n');
		ln_nmb++;

		vector<lexeme> lex = Lexical(line);	// Таблиця лексем
		ln_struct ln = Syntax(lex);			// Таблиця структури речення
		m_code code;						// Машинний код

		

		if (is_lex_error(lex))				// Лексична помилка
			_asm
		{
			jmp err;
		}

		// Всі можливі варіанти
		if (is_SEGMENT(ln, lex) && (active_seg == -1))
		{
			string new_seg = get_new_ident(ln, lex);

			// Якщо оголошуємо третій сегмент
			if ((t.DS.name == new_seg) || (t.CS.name == new_seg))
				goto err;

			if (t.DS.name.empty())
			{
				t.DS.name = new_seg;
				active_seg = 0;
			}
			else
			{
				if (t.CS.name.empty())
				{
					t.CS.name = new_seg;
					for (int i = 0; i < ins.size(); i++)
						t.err_lns.push_back(ins[i]);

					active_seg = 1;
				}
				else
				{
					active_seg = -1;
					_asm
					{
						jmp err;
					}
				}
			}

			offset = 0;
		}
		else
		if (is_ENDS(ln, lex) && (active_seg != -1) && (active_seg != 2))
		{
			string seg = lex[0].name;

			// Якщо закриваємо сегмент даних
			if (active_seg == 0)
			{
				active_seg = 2;
				if (t.DS.name != seg)
				{
					t.DS.size = -1;
					_asm
					{
						jmp err;
					}
				}
				t.DS.size = offset;
			}
			// Якщо закриваємо сегмент кодів
			if (active_seg == 1)
			{
				active_seg = 2;
				if (t.CS.name != seg)
				{
					t.CS.size = -1;
					_asm
					{
						jmp err;
					}
				}
				t.CS.size = offset;
			}
			active_seg = -1;
		}
		else
		if ((active_seg != -1) && (active_seg != 2) && ((is_DB(ln, lex) || is_DW(ln, lex) || is_DD(ln, lex) || is_LABEL(ln, lex))) && ops_bit(ln, lex, t))
		{
			// Нова ідентифікатор
			ident new_id;
			new_id.name = get_new_ident(ln, lex);

			// Пошук в таблиці вже існуючих
			for (int i = 0; i < t.ids.size(); i++)
			if (t.ids[i].name == new_id.name)
				_asm
			{
				jmp err;
			}

			// Знаходимо тип
			if (is_DB(ln, lex))
				new_id.type = 1;
			if (is_DW(ln, lex))
				new_id.type = 2;
			if (is_DD(ln, lex))
				new_id.type = 4;
			if (is_LABEL(ln, lex))
				new_id.type = 0;

			// Встановлюємо цифри
			new_id.offset = offset;
			if (active_seg == 0)
				new_id.seg_name = t.DS.name;
			if (active_seg == 1)
				new_id.seg_name = t.CS.name;

			t.ids.push_back(new_id);
		}
		else
		if (((active_seg == 0) || (active_seg == 1)) && ops_bit(ln, lex, t))
		{
			// Аналогічний пошук
			ident new_id;
			new_id.name = get_new_ident(ln, lex);

			for (int i = 0; i < t.ids.size(); i++)
			if (t.ids[i].name == new_id.name)
				_asm
			{
				jmp err;
			}

			new_id.type = 0;
			new_id.offset = offset;
			if (active_seg == 0)
				new_id.seg_name = t.DS.name;
			if (active_seg == 1)
				new_id.seg_name = t.CS.name;

			if (!new_id.name.empty())
				t.ids.push_back(new_id);
		}
		else
		if (!lex.empty() && !is_END(ln, lex))
			_asm
		{
			jmp err;
		}

		// Якщо оголошення мітки або змінної в сегменті кодів
		if (is_LABEL(ln, lex) || (active_seg != -1) && ops_bit(ln, lex, t) && !is_DB(ln, lex) && !is_DW(ln, lex) && !is_DD(ln, lex))
		if (active_seg == 0)
			ins.push_back(ln_nmb);

		if (ln_nmb == 4)
		{
			int x;
			x = 13;
		}

		// Отримуємо машинний код і зміщення
		code = machine_code(ln, lex, t, active_seg, offset);
		offset += code.size();

		// Якщо зустріли директиву END
		if (is_END(ln, lex))
		{
			if (t.CS.name.empty())
			{
				t.CS.name = t.DS.name;
				t.CS.size = t.DS.size;
				t.DS.name = "";
			}
			break;
		}

		continue;

		// Заносимо номери рядків з помилками
		_asm
		{
		err:
		}
		t.err_lns.push_back(ln_nmb);
	}


	f.close();
	g.close();
	return t;
}