#include <iostream>
#include "TScaner.h"
#include "TDiagram.h"

using namespace std;

int main(int argc, char* argv[]) {

    setlocale(LC_ALL, "Rus");      

    TScaner* sc = NULL;
    TDiagram* di = NULL;

    if (argc <= 1)
        sc = new TScaner("input.txt");
    else
        sc = new TScaner(argv[1]);

    int t;
    do {
        LEX lex;
        t = sc->scaner(lex);
        cout << "Тип = " << t << " Лексема = " << lex << endl;
    } while (t != TEnd); 

    sc->setUK(0);
    sc->setLine(1);

    cout << "\nРезультат работы синтаксического анализатора:" << endl;
    try {
        di = new TDiagram(sc);
        di->S(); 

        LEX l;
        t = sc->scaner(l);
        if (t == TEnd)
            cout << "Ok"<< endl;
        else
            sc->printError("Лишний текст в конце программы", "");

        if (sc != NULL) delete sc;
        if (di != NULL) delete di;
    }
    catch (const char* err) {
        cout << "Ошибка: " << err << endl;
        if (sc != NULL) delete sc;
        if (di != NULL) delete di;
    }
    return 0;
}