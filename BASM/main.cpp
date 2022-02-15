#include "Second.h"

int main()
{
	Files fls = get_files();	// Файли вхідного коду та лістингу
	cout << endl;

	if (fls.is_source && fls.is_listing)	// Якщо вказані коректні імена
	{
		Tables t = First(fls);	// Формуємо таблиці першого проходу
		Second(fls, t);			// Другий прохід

		// Виводимо таблиці і кількість помилок
		ofstream g(fls.lst_name, ios_base::app);
		g << endl << t << endl;
		g << "      " << t.err_lns.size() << " Errors" << endl;
		cout << endl << "      " << t.err_lns.size() << " Errors" << endl;

		g.close();
	}

	return 0;
}