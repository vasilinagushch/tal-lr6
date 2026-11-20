#pragma once

#include <iostream>
#include <string>
#include "defs.h"
#include "TScaner.h"

class TDiagram {
private:
    TScaner* scanner;  // указатель на лексический анализатор

    struct TokenInfo {
        int type;
        LEX lex;
    };
    TokenInfo cur, next;

    void nextToken();
    void match(int expected, const std::string& errorMsg);
    void error(const std::string& msg);

    void parseProgram();
    void parseDescription();
    void parseMainFunction();
    void parseDataDescription();
    void parseStructDescription();
    void parseType();
    void parseVariableList();
    void parseVariable();
    void parseCompoundOperator();
    void parseOperator();
    void parseAssignment();
    void parseDoWhile();
    void parseExpression();
    void parseEquality();
    void parseRelation();
    void parseAdditive();
    void parseMultiplicative();
    void parseUnary();
    void parsePrimary();
    void parseFieldAccess();
    void parseFunctionCall();

public:
    TDiagram(TScaner* sc); 
    void S();
};

