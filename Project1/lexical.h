#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<fstream>

#define MAX_IDENTIFERS 1000

#define COMMENT "some comments"

enum tokenType
{
	IF,ELSE,WHILE,READ,WRITE,INT,REAL,BREAK,											// Keywords
	LPARENT,RPARENT,LBRACKET,RBRACKET,LBRACE,RBRACE,SEMI,COMMA,							// Community sites
	PLUS,MINUS,MULT,DIV,ASSIGN,GT,LT,EQ,NEQ,											// Binray operators
	NGT, PST,																			// Unary operators
	IDENTIFER,																			// Identifers
	INUM,RNUM,																			// Numbers
	UNIDENTIFIED,UNCLOSESITE,WNUM,														// Errors
	NONE,																				// Not get the type yet
};


std::string tokenEnum2String(tokenType type);
bool lexicalAnalysis(std::string fileName,std::vector<std::string>& outputs);

