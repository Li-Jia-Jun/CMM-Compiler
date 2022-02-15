#include"interpret.h"


void interpretStmt(int level,treeNode* node);
void interpretIf(int level,treeNode* ifNode);
void interpretWhile(int level,treeNode* whileNode);
void interpretDeclare(int level,treeNode* declareNode);
void interpretAssign(int level,treeNode* assignNode);
void interpretRead(int level,treeNode* readNode);
void interpretWrite(int level,treeNode* writeNode);
void interpretBlock(int level,treeNode* blockNode);
Value interpretExp(int level,treeNode* expNode);
Value interpretLogicalExp(int level,treeNode* logicalExpNode);
Value interpretAdditiveExp(int level,treeNode* additiveExpNode);
Value interpretTerm(int level,treeNode* termExpNode);
Value interpretFactor(int level,treeNode* factorNode);
Symbol* interpretVar(int level,treeNode* varNode);
std::string SVType2String(SVType type);



SymbolTable symTable;
bool hasBreak = false;

void interpret(std::vector<treeNode*> &inputs)
{
	try
	{
		int level = 0;
		int size = inputs.size();
		for (int i = 0; i < size; i++)
		{
			interpretStmt(level,inputs.at(i));
		}
		symTable.deleteSymbols(0);
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void interpretStmt(int level, treeNode* node)
{
	switch (node->stmt)
	{
		case treeNodeStmt::IF_STMT:	interpretIf(level, node); break;
		case treeNodeStmt::WHILE_STMT:	interpretWhile(level, node); break;
		case treeNodeStmt::DECLARE_STMT:	interpretDeclare(level, node); break;
		case treeNodeStmt::ASSIGN_STMT:	interpretAssign(level, node); break;
		case treeNodeStmt::READ_STMT:	interpretRead(level, node); break;
		case treeNodeStmt::WRITE_STMT:	interpretWrite(level, node); break;
		case treeNodeStmt::BLOCK_STMT:	interpretBlock(level, node); break;
		case treeNodeStmt::BREAK_STMT:	hasBreak = true; break;
		default: break;
	}
}

void interpretIf(int level, treeNode* ifNode)
{
	Value v = interpretExp(level, ifNode->mLeft);

	if(v.type == SVType::VALUE_TRUE || v.getCastBool())
	{
		interpretStmt(level, ifNode->mMiddle);
	}
	else if(ifNode->mRight != NULL)
	{
		interpretStmt(level, ifNode->mRight);
	}
}

void interpretWhile(int level, treeNode* whileNode)
{
	Value condition = interpretExp(level, whileNode->mLeft);
	while (condition.type == SVType::VALUE_TRUE || condition.getCastBool())
	{
		interpretStmt(level, whileNode->mMiddle);
		if(hasBreak) 
		{
			hasBreak = false;
			break;
		}
		condition = interpretExp(level,whileNode->mLeft);
	}
}

void interpretDeclare(int level, treeNode* declareNode)
{	
	Symbol *sym = NULL;
	if (declareNode->mRight != NULL)	// Array declaration
	{
		sym = new Symbol();
		int count;

		// Leftest demension
		Value value = interpretExp(level,declareNode->mRight);
		if(value.type != SVType::SV_INT)
		{
			throw std::runtime_error("At line " + std::to_string(declareNode->mRight->line) + 
				": Array index should be integer.");
		}
		else
		{
			count = value.mInt;
			sym->arrayInfo.push_back(value.mInt);
		}

		// Other demensions
		treeNode* temp = declareNode->mNext;
		Value tempV;
		while (temp != NULL)
		{
			tempV = interpretExp(level, temp->mRight);
			if (tempV.type != SVType::SV_INT)
			{
				throw std::runtime_error("At line " + std::to_string(temp->line) +
					": array index should be integer.");
			}
			else
			{
				count *= tempV.mInt;
				sym->arrayInfo.push_back(tempV.mInt);
			}
			temp = temp->mNext;
		}

		Symbol *arrayHead = NULL;
		if (declareNode->mTokenType== tokenType::INT)	// Int array
		{
			sym->arrayCount = count;
			sym->name = declareNode->mLeft->content;
			sym->value.type = SVType::SV_INT;
			sym->value.mInt = 0;
			arrayHead = sym;

			for(int i = 1; i < count; i++)
			{
				sym->arrayNext = new Symbol();
				sym = sym->arrayNext;
				sym->name = declareNode->mLeft->content;
				sym->value.type = SVType::SV_INT;
				sym->value.mInt = 0;
			}
		}
		else											// Real array
		{
			sym = new Symbol();
			sym->arrayCount = count;
			sym->name = declareNode->content;
			sym->value.type = SVType::SV_REAL;
			sym->value.mDouble = 0.0;
			arrayHead = sym;

			for (int i = 1; i < count; i++)
			{
				sym->arrayNext = new Symbol();
				sym = sym->arrayNext;
				sym->name = declareNode->content;
				sym->value.type = SVType::SV_REAL;
				sym->value.mDouble = 0.0;
			}
		}

		symTable.registerSymbol(arrayHead);
	}
	else	// Single variable or varList
	{
		treeNode* temp = declareNode->mLeft;
		do
		{
			sym = new Symbol();
			sym->level = level;
			sym->name = temp->content;

			if (temp->mMiddle != NULL)	// Assign part
			{
				sym->value = interpretExp(level, temp->mMiddle);
			}

			if (declareNode->mTokenType == tokenType::INT)
			{
				sym->type = SVType::SV_INT;
			}
			else
			{
				sym->type = SVType::SV_REAL;
			}
			sym->value.type = sym->type;

			symTable.registerSymbol(sym);

			temp = temp->mNext;
		} while (temp != NULL);
	}
}

void interpretAssign(int level, treeNode* assignNode)
{
	Value v = interpretExp(level, assignNode->mMiddle);
	Symbol* sym = interpretVar(level, assignNode->mLeft);

	if(sym->type == v.type)
	{
		sym->value = v;
	}
	else if (sym->type == SVType::SV_REAL && v.type == SVType::SV_INT)
	{
		sym->value.mInt = v.mInt;
		sym->value.mDouble = v.mDouble;
	}
	else if(sym->type ==  SVType::SV_REAL && v.type == SVType::SV_INT)	// Safety cast
	{
		sym->value.mDouble = v.mInt;
	}
	else
	{
		throw std::runtime_error("At line " + std::to_string(assignNode->line) + 
			": Unsupport cast." );
	}
}

void interpretRead(int level, treeNode* readNode)
{
	Symbol* sym = interpretVar(level, readNode->mLeft);
	std::string str;

INPUT:
	std::cout<<"Please input " + SVType2String(sym->type) + " value for " + sym->name<<": ";
	std::getline(std::cin, str);

	if (sym->type == SVType::SV_INT)
	{
		int size = std::strlen(str.c_str());
		for (int i = 0; i < size; i++)
		{
			if (!isdigit(str.at(i)))
			{
				std::cout << "Invalid Input. Please try again." << std::endl;
				goto INPUT;
			}
		}

		int i = std::atoi(str.c_str());
		sym->value.mInt = i;
	}
	else if (sym->type == SVType::SV_REAL)
	{
		int size = std::strlen(str.c_str());
		for (int i = 0; i < size; i++)
		{
			std::cout << "Invalid Input. Please try again." << std::endl;
			goto INPUT;
		}

		double d = std::atof(str.c_str());
		sym->value.mDouble = d;
	}
}

void interpretWrite(int level, treeNode* writeNode)
{
	Value value = interpretExp(level, writeNode->mLeft);
	int size = 0, count;
	switch(value.type)
	{
		case SVType::SV_INT: std::cout << value.mInt << std::endl; break;
		case SVType::SV_REAL: std::cout << value.mDouble << std::endl; break;
		default: break;
	}
}

void interpretBlock(int level, treeNode* blockNode)
{
	++level;

	treeNode* node = blockNode->mNext;
	do
	{
		interpretStmt(level, node);
		node = node->mNext;
	}
	while(node != NULL);

	// Delete symbols when existing a block
	symTable.deleteSymbols(level);
}

Value interpretExp(int level, treeNode* expNode)
{
	// An expression could be LogicalExp, Term, AdditiveExp or Factor

	switch (expNode->stmt)
	{
		case treeNodeStmt::LOGICAL_EXP_STMT: return interpretLogicalExp(level, expNode);
		case treeNodeStmt::ADDITIVE_EXP_STMT: return interpretAdditiveExp(level, expNode);
		case treeNodeStmt::TERM_STMT:	return interpretTerm(level, expNode);
		case treeNodeStmt::FACTOR_STMT: return interpretFactor(level, expNode);
		default: Value v; v.type = SVType::VALUE_NONEXIST; return v;
	}
}

Value interpretLogicalExp(int level,treeNode* logicalExpNode)
{
	Value left, right, result;
	left = interpretExp(level, logicalExpNode->mLeft);
	right = interpretExp(level,logicalExpNode->mRight);

	if(logicalExpNode->mMiddle->mTokenType == tokenType::GT)
	{
		if(left.mInt + left.mDouble > right.mInt + right.mDouble)
		{
			result.type = SVType::VALUE_TRUE;
		}
		else
		{
			result.type = SVType::VALUE_FALSE;
		}
	}
	else if(logicalExpNode->mMiddle->mTokenType == tokenType::LT)
	{
		if ((double)left.mInt + left.mDouble < (double)right.mInt + right.mDouble)
		{
			result.type = SVType::VALUE_TRUE;
		}
		else
		{
			result.type = SVType::VALUE_FALSE;
		}
	}
	else if (logicalExpNode->mMiddle->mTokenType == tokenType::NEQ)
	{
		if(IS_DOUBLE_NOT_ZERO(left.mInt + left.mDouble - right.mInt - right.mDouble))
		{
			result.type = SVType::VALUE_TRUE;
		}
		else
		{
			result.type = SVType::VALUE_FALSE;
		}
	}
	else // Equal
	{
		if (IS_DOUBLE_NOT_ZERO(left.mInt + left.mDouble - right.mInt - right.mDouble))
		{
			result.type = SVType::VALUE_FALSE;
		}
		else
		{
			result.type = SVType::VALUE_TRUE;
		}
	}

	return result;
}

Value interpretAdditiveExp(int level, treeNode* additiveExpNode)
{
	Value left,right,result;
	left = interpretExp(level, additiveExpNode->mLeft);
	right = interpretExp(level, additiveExpNode->mRight);
	
	if(left.type == SVType::SV_REAL || right.type == SVType::SV_REAL)
	{
		result.type = SVType::SV_REAL;
		if(additiveExpNode->mMiddle->mTokenType == tokenType::PLUS)
		{
			result.mDouble = left.mInt + left.mDouble + right.mInt + right.mDouble;
		}
		else // Minus
		{
			result.mDouble = left.mInt + left.mDouble - right.mInt - right.mDouble;
		}	
	}
	else
	{
		result.type = SVType::SV_INT;
		if (additiveExpNode->mMiddle->mTokenType == tokenType::PLUS)
		{
			result.mInt = left.mInt + right.mInt;
		}
		else // Minus
		{
			result.mInt = left.mInt - right.mInt;
		}
	}

	return result;
}

Value interpretTerm(int level, treeNode* termExpNode)
{
	Value left,right,result;
	left = interpretExp(level, termExpNode->mLeft);
	right = interpretExp(level, termExpNode->mRight);

	if (left.type == SVType::SV_REAL && right.type == SVType::SV_REAL)
	{
		result.type = SVType::SV_REAL;
		if (termExpNode->mMiddle->mTokenType == tokenType::MULT)
		{
			result.mDouble = left.mDouble * right.mDouble;
		}
		else // Divide
		{
			if(!IS_DOUBLE_NOT_ZERO(right.mDouble))
			{		
				throw std::runtime_error("At line " + std::to_string(termExpNode->line) +
					": Divider must be non-zero.");
			}
			else
			{
				result.mDouble = left.mDouble / right.mDouble;
			}
		}
	}
	else if (left.type == SVType::SV_INT && right.type == SVType::SV_REAL)
	{
		result.type = SVType::SV_REAL;
		if (termExpNode->mMiddle->mTokenType == tokenType::MULT)
		{
			result.mDouble = left.mInt * right.mDouble;
		}
		else // Divide
		{
			if (!IS_DOUBLE_NOT_ZERO(right.mDouble))
			{
				throw std::runtime_error("At line " + std::to_string(termExpNode->line) +
					": Divider must be non-zero.");
			}
			else
			{
				result.mDouble = left.mInt / right.mDouble;
			}
		}
	}
	if (left.type == SVType::SV_REAL && right.type == SVType::SV_INT)
	{
		result.type = SVType::SV_REAL;
		if (termExpNode->mMiddle->mTokenType == tokenType::MULT)
		{
			result.mDouble = left.mDouble * right.mInt;
		}
		else // Divide
		{
			if (right.mInt == 0)
			{
				throw std::runtime_error("At line " + std::to_string(termExpNode->line) +
					": Divider must be non-zero.");
			}
			else
			{
				result.mDouble = left.mDouble / right.mInt;
			}
		}
	}
	else
	{
		result.type = SVType::SV_INT;
		if (termExpNode->mMiddle->mTokenType == tokenType::MULT)
		{
			result.mInt = left.mInt * right.mInt;
		}
		else // Divide
		{
			if (right.mInt == 0)
			{
				throw std::runtime_error("At line " + std::to_string(termExpNode->line) +
					": Divider must be non-zero.");
			}
			else
			{
				result.mInt = left.mInt / right.mInt;
			}
		}
	}

	return result;
}

Value interpretFactor(int level, treeNode* factorNode)
{
	bool isNegative = false;
	if(factorNode->mTokenType == tokenType::NGT)
		isNegative = true;

	if(factorNode->mLeft->stmt == treeNodeStmt::NUMBER_STMT)
	{
		Value v;
		if(factorNode->mLeft->mTokenType == tokenType::INUM)
		{
			v.type = SVType::SV_INT;
			v.mInt = std::atoi(factorNode->mLeft->content.c_str());
		}
		else
		{
			v.type = SVType::SV_REAL;
			v.mDouble = std::atof(factorNode->mLeft->content.c_str());
		}
		return v;
	}
	else if(factorNode->mLeft->stmt == treeNodeStmt::VAR_STMT)
	{
		if(isNegative)
			return interpretVar(level, factorNode->mLeft)->value.getNegativeValue();
		else
			return interpretVar(level,factorNode->mLeft)->value;
	}
	else // (Exp) is regarded as factor
	{
		if (isNegative)
			return interpretExp(level, factorNode->mLeft).getNegativeValue();
		else
			return interpretExp(level, factorNode->mLeft);
	}
}

Symbol* interpretVar(int level, treeNode* varNode)
{
	Symbol* sym = symTable.findSymbol(varNode->content);

	if(sym == symTable.nonExistSym)
	{
		throw std::runtime_error("At line " + std::to_string(varNode->line) +
			": A variable must be declared before being used.");
	}

	if(varNode->mRight == NULL)	// Single element
	{		
		return sym;
	}
	else						// Array element
	{
		// Collect indexes for array
		std::vector<int> indexes;
		Value v;
		treeNode* temp = varNode->mRight;
		v = interpretExp(level,temp);
		if (v.type != SVType::SV_INT)
		{
			throw std::runtime_error("At line " + std::to_string(varNode->line) +
				": Array index must be integer.");
		}
		indexes.push_back(v.mInt);
		temp = varNode->mNext;
		while (temp != NULL)
		{
			v = interpretExp(level, temp->mRight);
			if (v.type != SVType::SV_INT)
			{
				throw std::runtime_error("At line " + std::to_string(varNode->line) +
					": Array index must be integer.");
			}
			indexes.push_back(v.mInt);
			temp = temp->mNext;
		}

		if(sym->arrayInfo.size() != indexes.size())
		{
			throw std::runtime_error("At line " + std::to_string(varNode->line) +
				": Incorrect array index count.");
		}

		int size = indexes.size(), count = 0;
		for(int i = 0; i < size; i++)
		{
			if(indexes.at(i) < 0 || indexes.at(i) >= sym->arrayInfo.at(i))
			{
				throw std::runtime_error("At line " + std::to_string(varNode->line) +
					": Array index out of bound.");
			}
			else
			{
				// Turn multple demension indexes into one demension
				int temp = indexes.at(i);
				for(int j = i + 1; j < size; j++)
					 temp *= sym->arrayInfo[j];
				count += temp;
			}
		}
		
		Symbol *tempS = sym;
		for(int i = 0; i < count; i++)
			tempS = tempS->arrayNext;

		return tempS;
	}
}

std::string SVType2String(SVType type)
{
	std::string str = "";
	switch (type)
	{
		case SV_INT: str = "int"; break;
		case SV_REAL: str = "real"; break;
		default: break;
	}

	return str;
}