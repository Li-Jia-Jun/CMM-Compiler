#pragma once
#include<iostream>
#include<vector>
#include"parse.h"

#define MIN_VALUE 1e-8
#define IS_DOUBLE_NOT_ZERO(d)  (std::abs(d) > MIN_VALUE)

enum SVType
{
	SV_INT,SV_REAL,							// For both symbol and value
	VALUE_TRUE,VALUE_FALSE, VALUE_NONEXIST	// For value type only
};

struct Value
{
	SVType type;
	int mInt;
	double mDouble;

	Value()
	{
		type = SVType::VALUE_NONEXIST;
		mInt = 0;
		mDouble = 0.0;
	}

	bool getCastBool()
	{
		if(type == SVType::SV_INT)
			return mInt != 0;
		else if(type == SVType::SV_REAL)
			return IS_DOUBLE_NOT_ZERO(mDouble);
		else
			return false;
	}

	Value getNegativeValue()
	{
		Value v;
		v.type = type;
		v.mInt = -mInt;
		v.mDouble = -mDouble;
		return v;
	}
};

struct Symbol
{
	SVType type;
	std::string name;
	int level;
	Value value;

	void setType(SVType type)
	{
		this->type = type;
		if(type == SVType::SV_INT || SVType::SV_REAL)
			this->value.type = type;
	}

	int arrayCount;					// Element count of array
	std::vector<int> arrayInfo;		// Demension values of array
	Symbol* arrayNext;				// Next element in the array

	Symbol* next;					// Next element in the symbol table
};

struct SymbolTable
{
	std::vector<Symbol*> symbols;
	Symbol* nonExistSym; 

	SymbolTable()
	{
		nonExistSym = new Symbol();
		nonExistSym->name = "#";
	}

	Symbol* registerSymbol(Symbol* sym)
	{
		Symbol* p = NULL;
		for (int i = 0; i < symbols.size(); i++)	// Symbol of the same name
		{
			p = symbols.at(i);
			if (sym->name == p->name)
			{
				if (sym->level > p->level)
				{
					sym->next = p;
					symbols.at(i) = sym;
					return sym;
				}
				else
				{
					return nonExistSym;
				}
			}
		}

		// New symbol
		symbols.push_back(sym);

		return sym;
	}

	void deleteSymbols(int level)
	{
		int size = symbols.size();
		for(int i = 0; i < size; i++)
		{
			Symbol *sym = symbols.at(i);
			if(sym->level == level)
			{
				symbols.at(i) == sym->next;
				delete sym; sym = NULL;
			}
		}
	}

	Symbol* findSymbol(std::string name)
	{
		int size = symbols.size();
		for (int i = 0; i < size; i++)
		{
			Symbol* sym = symbols.at(i);
			if(sym->name == name)
				return sym;
		}

		return nonExistSym;
	}
};

void interpret(std::vector<treeNode*> &inputs);