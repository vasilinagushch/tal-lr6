#include "TDiagram.h"

TDiagram::TDiagram(TScaner* sc) : scanner(sc) {
    cur.type = scanner->scaner(cur.lex);
    next.type = scanner->scaner(next.lex);
}

void TDiagram::nextToken() {
    std::cerr << "Тип =  " << cur.type << " Лексема =  " << cur.lex << "\n";
    cur = next;
    if (cur.type == TErr) {
        throw "Лексическая ошибка";
    }
    next.type = scanner->scaner(next.lex);
}

void TDiagram::error(const std::string& msg) {
    static char buffer[512];
    snprintf(buffer, sizeof(buffer), "Синтаксическая ошибка (строка %d): %s. Получено: [%s]",
        scanner->getLine(), msg.c_str(), cur.lex);
    throw buffer;
}

void TDiagram::match(int expected, const std::string& errorMsg) {
    if (cur.type == expected) {
        nextToken();
    }
    else {
        error(errorMsg);
    }
}

void TDiagram::S() {
    parseProgram();
}

void TDiagram::parseProgram() {
    while (cur.type != TEnd) {
        parseDescription();
    }
}

// <описание> -> <описание структуры> | <описание данных> | <главная функция>
void TDiagram::parseDescription() {
    if (cur.type == TStruct) {
        parseStructDescription();
    }
    else if (cur.type == TInt) {
        if (next.type == TMain) {
            parseMainFunction();
        }
        else {
            parseDataDescription();
        }
    }
    else if (cur.type == TShort || cur.type == TLong || cur.type == TDouble || cur.type == TIdent) {
        parseDataDescription();
    }
    else {
        error("Ожидалось описание (struct, тип данных или main)");
    }
}

// <главная функция> -> int main ( ) <составной оператор>
void TDiagram::parseMainFunction() {
    match(TInt, "Ожидался тип 'int'");
    match(TMain, "Ожидалось 'main'");
    match(TLParen, "Ожидалась '('");
    match(TRParen, "Ожидалась ')'");
    parseCompoundOperator();
}

// <описание данных> -> <тип> <список переменных> ;
void TDiagram::parseDataDescription() {
    parseType();
    parseVariableList();
    match(TSemicolon, "Ожидалась ';' после объявления переменных");
}

// <описание структуры> -> struct <имя> { <список полей> } ;
void TDiagram::parseStructDescription() {
    match(TStruct, "Ожидалось 'struct'");
    match(TIdent, "Ожидалось имя структуры");
    match(TLBrace, "Ожидалась '{'");
    while (cur.type != TRBrace && cur.type != TEnd) {
        parseDataDescription();
    }
    match(TRBrace, "Ожидалась '}'");
    match(TSemicolon, "Ожидалась ';' после структуры");
}

// <тип> -> int | short | long | double | <имя>
void TDiagram::parseType() {
    if (cur.type == TInt || cur.type == TDouble || cur.type == TIdent ||
        cur.type == TShort || cur.type == TLong) {
        nextToken();
    }
    else {
        error("Ожидался тип данных");
    }
}

// <список переменных> -> <переменная> { , <переменная> }
void TDiagram::parseVariableList() {
    parseVariable();
    while (cur.type == TComma) {
        nextToken();
        parseVariable();
    }
}

// <переменная> -> <имя> [ = <выражение> ]
void TDiagram::parseVariable() {
    match(TIdent, "Ожидалось имя переменной");
    if (cur.type == TAssign) {
        nextToken();
        parseExpression();
    }
}

// <составной оператор> -> { <описания и операторы> }
void TDiagram::parseCompoundOperator() {
    match(TLBrace, "Ожидалась '{'");
    while (cur.type != TRBrace && cur.type != TEnd) {

        // Добавляем отдельную ветку для struct
        if (cur.type == TStruct) {
            parseStructDescription();
        }
        else if ( cur.type == TInt || cur.type == TShort ||
            cur.type == TLong || cur.type == TDouble) {
            parseDataDescription();
        }
        else if (cur.type == TIdent && next.type == TIdent) {
            parseDataDescription();
        }
        else {
            parseOperator();
        }
    }
    match(TRBrace, "Ожидалась '}'");
}

// <оператор> -> ; | <составной> | <присваивание> | <do-while>
void TDiagram::parseOperator() {
    if (cur.type == TSemicolon) {
        nextToken(); // Пустой оператор
    }
    else if (cur.type == TLBrace) {
        parseCompoundOperator();
    }
    else if (cur.type == TDo) {
        parseDoWhile();
    }
    else if (cur.type == TIdent) {
        parseAssignment();
    }
    else {
        error("Ожидался оператор");
    }
}

// <оператор присваивания> -> <доступ к полю> = <выражение> ;
void TDiagram::parseAssignment() {
    parseFieldAccess();
    match(TAssign, "Ожидался оператор '='");
    parseExpression();
    match(TSemicolon, "Ожидалась ';' после присваивания");
}

// <оператор do-while> -> do <оператор> while ( <выражение> ) ;
void TDiagram::parseDoWhile() {
    match(TDo, "Ожидалось 'do'");
    parseOperator();
    match(TWhile, "Ожидалось 'while'");
    match(TLParen, "Ожидалась '('");
    parseExpression();
    match(TRParen, "Ожидалась ')'");
    match(TSemicolon, "Ожидалась ';' после while");
}

// ВЫРАЖЕНИЯ 
void TDiagram::parseExpression() { parseEquality(); }

void TDiagram::parseEquality() {
    parseRelation();
    while (cur.type == TEq || cur.type == TNeq) {
        nextToken(); parseRelation();
    }
}

void TDiagram::parseRelation() {
    parseAdditive();
    while (cur.type == TLt || cur.type == TGt || cur.type == TLe || cur.type == TGe) {
        nextToken(); parseAdditive();
    }
}

void TDiagram::parseAdditive() {
    parseMultiplicative();
    while (cur.type == TPlus || cur.type == TMinus) {
        nextToken(); parseMultiplicative();
    }
}

void TDiagram::parseMultiplicative() {
    parseUnary();
    while (cur.type == TMult || cur.type == TDiv || cur.type == TMod) {
        nextToken(); parseUnary();
    }
}

// Правая рекурсия реализуется прямым вызовом
void TDiagram::parseUnary() {
    if (cur.type == TPlus || cur.type == TMinus) {
        nextToken();
        parseUnary();
    }
    else {
        parsePrimary();
    }
}

void TDiagram::parsePrimary() {
    if (cur.type == TIdent) {
        if (next.type == TLParen) {
            parseFunctionCall();
        }
        else {
            parseFieldAccess();
        }
    }
    else if (cur.type == TConstInt10 || cur.type == TConstInt16 || cur.type == TConstFloat) {
        nextToken();
    }
    else if (cur.type == TLParen) {
        nextToken();
        parseExpression();
        match(TRParen, "Ожидалась ')'");
    }
    else {
        error("Ожидался операнд");
    }
}

void TDiagram::parseFieldAccess() {
    match(TIdent, "Ожидалось имя");
    while (cur.type == TDot) {
        nextToken();
        match(TIdent, "Ожидалось имя поля");
    }
}

void TDiagram::parseFunctionCall() {
    match(TIdent, "Ожидалось имя функции");
    match(TLParen, "Ожидалась '('");
    if (cur.type != TRParen) {
        parseExpression();
        while (cur.type == TComma) {
            nextToken();
            parseExpression();
        }
    }
    match(TRParen, "Ожидалась ')'");
}