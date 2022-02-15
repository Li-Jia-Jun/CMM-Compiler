#include"parse.h"
#include"lexical.h"
#include<vector>

bool uncloseBrace = false;
bool uncloseBrasket = false;
bool uncloseParent = false;

std::vector<treeNode*>* outputsLocal;
std::vector<std::string>* inputsLocal;
int next = 0;

bool checkNextToken(tokenType type);
int getNextTokenLine();
void consumeNextToken(tokenType nextTypeShouldBe);
tokenType getNextTokenType();
std::string getNextToken();


treeNode* parseStmt();
treeNode* parseIfStmt();
treeNode* parseWhileStmt();
treeNode* parseDeclareStmt();
treeNode* parseReadStmt();
treeNode* parseWriteStmt();
treeNode* parseBlockStmt();
treeNode* parseAssignStmt();
treeNode* parseExpStmt();
treeNode* parseAdditiveExpStmt();
treeNode* parseTermStmt();
treeNode* parseFactor();
treeNode* parseVar();
treeNode* parseNumber();
treeNode* parseLogicalOp();
treeNode* parseMultiplyOp();
treeNode* parseAdditiveOp();


bool checkNextToken(tokenType type)
{
	if(next > inputsLocal->size() - 3)
		return false;

	return getNextTokenType() == type;
}

tokenType getLastTokenType()
{
	return (tokenType)std::atoi(inputsLocal->at(next - 2).c_str());
}

tokenType getNextTokenType()
{
	if (next > inputsLocal->size() - 3)
		return tokenType::NONE;
	
	return  (tokenType)std::atoi(inputsLocal->at(next + 1).c_str());
}

int getNextTokenLine()
{
	if (next > inputsLocal->size() - 3)
		return -1;

	return std::atoi(inputsLocal->at(next + 2).c_str());
}

std::string getNextToken()
{
	if (next > inputsLocal->size() - 3)
		return NULL;

	return inputsLocal->at(next);
}

void consumeNextToken(tokenType nextTypeShouldBe)
{
	if(next > inputsLocal->size() - 3)
	{
		return;
	}

	if(!checkNextToken(nextTypeShouldBe))
	{
		throw std::runtime_error("At line" + inputsLocal->at(next + 2) + ": next token should be " + tokenEnum2String(nextTypeShouldBe));
	}
	else
	{
		if(getNextTokenType() == tokenType::LBRACE)
			uncloseBrace = true;
		else if (getNextTokenType() == tokenType::RBRACE)
			uncloseBrace = false;
		if (getNextTokenType() == tokenType::LBRACKET)
			uncloseBrasket = true;
		else if (getNextTokenType() == tokenType::RBRACKET)
			uncloseBrasket = false;
		if (getNextTokenType() == tokenType::LPARENT)
			uncloseParent = true;
		else if (getNextTokenType() == tokenType::RPARENT)
			uncloseParent = false;

		next += 3;	
	}
}

treeNode* parseStmt()
{
	tokenType type = getNextTokenType();

	switch (type)
	{
		case tokenType::IF:	return parseIfStmt();
		case tokenType::WHILE: return parseWhileStmt();
		case tokenType::READ: return parseReadStmt();
		case tokenType::WRITE: return parseWriteStmt();
		case tokenType::INT: 
		case tokenType::REAL: return parseDeclareStmt(); 
		case tokenType::LBRACE: return parseBlockStmt();
		case tokenType::IDENTIFER: return parseAssignStmt();
		case tokenType::BREAK:	return new treeNode(treeNodeStmt::BREAK_STMT);
		default: throw std::runtime_error("Unexpected token at line" + getNextTokenLine());
	}
}

treeNode* parseIfStmt()
{
	treeNode* node = new treeNode(treeNodeStmt::IF_STMT);

	consumeNextToken(tokenType::IF);
	consumeNextToken(tokenType::LPARENT);
	node->mLeft = parseExpStmt();
	consumeNextToken(tokenType::RPARENT);
	node->mMiddle = parseBlockStmt();

	// 'else' is an option
	if(checkNextToken(tokenType::ELSE))
	{
		consumeNextToken(tokenType::ELSE);
		node->mRight = parseBlockStmt();
	}

	return node;
}

treeNode* parseWhileStmt()
{
	treeNode* node = new treeNode(treeNodeStmt::WHILE_STMT);

	consumeNextToken(tokenType::WHILE);
	consumeNextToken(tokenType::LPARENT);
	node->mLeft = parseExpStmt();
	consumeNextToken(tokenType::RPARENT);
	node->mMiddle = parseBlockStmt();

	return node;
}

treeNode* parseDeclareStmt()
{
	// This function is a bit different...

	treeNode* node = new treeNode(treeNodeStmt::DECLARE_STMT);
	treeNode* varNode = new treeNode(treeNodeStmt::VAR_STMT);

	tokenType type = getNextTokenType();
	if(type == tokenType::INT || type == tokenType::REAL)
	{
		node->mTokenType = type;
		node->line = getNextTokenLine();
		consumeNextToken(type);

		if(checkNextToken(tokenType::IDENTIFER))
		{
			varNode->mTokenType = getNextTokenType();
			varNode->content = getNextToken();
			varNode->line = getNextTokenLine();
			consumeNextToken(tokenType::IDENTIFER);
		}
		else
		{
			throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be " + tokenEnum2String(tokenType::IDENTIFER));
		}

		type = getNextTokenType();
		if (type == tokenType::ASSIGN)
		{
			consumeNextToken(tokenType::ASSIGN);
			varNode->mMiddle = parseExpStmt();
		}
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be " +
			tokenEnum2String(tokenType::INT) + " or " + tokenEnum2String(tokenType::REAL));
	}
	
	type = getNextTokenType();
	treeNode* newNode;
	if (type == tokenType::COMMA)	// Variable list
	{
		do
		{
			consumeNextToken(tokenType::COMMA);
			if (checkNextToken(tokenType::IDENTIFER))
			{			
				newNode = new treeNode(VAR_STMT);
				newNode->mTokenType = getNextTokenType();
				newNode->content = getNextToken();
				newNode->line = getNextTokenLine();
				consumeNextToken(tokenType::IDENTIFER);
			}
			else
			{
				throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be " + tokenEnum2String(tokenType::IDENTIFER));
				break;
			}

			type = getNextTokenType();
			if (type == tokenType::ASSIGN)
			{
				consumeNextToken(tokenType::ASSIGN);
				newNode->mMiddle = parseExpStmt();
			}

			// Attach newNode to the end of mNext list
			treeNode* temp = varNode;
			while (temp->mNext != NULL) temp = temp->mNext;
			temp->mNext = newNode;

		} while (checkNextToken(tokenType::COMMA));
	}
	else if(type == tokenType::LBRACKET)	// Array
	{
		treeNode* temp = node;
		do
		{
			if(temp->mRight != NULL)
			{
				temp->mNext = new treeNode();
				temp = temp->mNext;
			}
			consumeNextToken(tokenType::LBRACKET);
			temp->mRight = parseExpStmt();
			consumeNextToken(tokenType::RBRACKET);
		} while (getNextTokenType() == tokenType::LBRACKET);	
	}

	consumeNextToken(tokenType::SEMI);

	node->mLeft = varNode;
	return node;
}

treeNode* parseReadStmt()
{
	treeNode* node = new treeNode(treeNodeStmt::READ_STMT);
	node->line = getNextTokenLine();

	consumeNextToken(tokenType::READ);
	node->mLeft = parseVar();
	consumeNextToken(tokenType::SEMI);

	return node;
}

treeNode* parseWriteStmt()
{
	treeNode* node = new treeNode(treeNodeStmt::WRITE_STMT);
	node->line = getNextTokenLine();

	consumeNextToken(tokenType::WRITE);
	node->mLeft = parseExpStmt();
	consumeNextToken(tokenType::SEMI);

	return node;
}

treeNode* parseBlockStmt()
{
	treeNode* head = new treeNode(treeNodeStmt::BLOCK_STMT);
	if(getNextTokenType() == tokenType::LBRACE)		// Multiple statements block
	{
		consumeNextToken(LBRACE);

		head->mNext = parseStmt();
		treeNode* node = head->mNext;
		while (!checkNextToken(tokenType::RBRACE))
		{
			node->mNext = parseStmt();
			node = node->mNext;
		}

		consumeNextToken(tokenType::RBRACE);
	}
	else											// Single statement block
	{
		head->mNext = parseStmt();
	}

	return head;
}

treeNode* parseAssignStmt()
{
	treeNode* node = new treeNode(treeNodeStmt::ASSIGN_STMT);
	node->line = getNextTokenLine();

	node->mLeft = parseVar();
	consumeNextToken(tokenType::ASSIGN);
	node->mMiddle = parseExpStmt();
	consumeNextToken(tokenType::SEMI);

	return node;
}

treeNode* parseExpStmt()
{
	// Let's assume it is logical expression
	// LogicalExp => AdditiveExp LogicalOp AdditiveExp | AdditiveExp

	treeNode* leftNode = parseAdditiveExpStmt();

	if(checkNextToken(tokenType::GT) || checkNextToken(tokenType::LT) || 
		checkNextToken(tokenType::EQ) || checkNextToken(tokenType::NEQ))
	{
		treeNode* node = new treeNode(treeNodeStmt::LOGICAL_EXP_STMT);
		node->mLeft = leftNode;
		node->line = getNextTokenLine();
		node->mMiddle = parseLogicalOp();
		node->mRight = parseAdditiveExpStmt();

		return node;
	}

	return leftNode;
}

treeNode* parseAdditiveExpStmt()
{	
	// AdditiveExp => Term AdditiveOp AdditiveExp | Term

	treeNode* leftNode = parseTermStmt();

	if (checkNextToken(tokenType::PLUS) || checkNextToken(tokenType::MINUS))
	{
		treeNode* node = new treeNode(treeNodeStmt::ADDITIVE_EXP_STMT);
		node->mLeft = leftNode;
		node->line = getNextTokenLine();
		node->mMiddle = parseAdditiveOp();
		node->mRight = parseAdditiveExpStmt();

		return node;
	}

	return leftNode;
}

treeNode* parseTermStmt()
{
	// Term => Factor MultiplyOp Term | Factor

	treeNode* leftNode = parseFactor();

	if (checkNextToken(tokenType::MULT) || checkNextToken(tokenType::DIV))
	{
		treeNode* node = new treeNode(treeNodeStmt::TERM_STMT);
		node->line = getNextTokenLine();
		node->mLeft = leftNode;
		node->mMiddle = parseMultiplyOp();
		node->mRight = parseTermStmt();

		return node;
	}

	return leftNode;
}

treeNode* parseFactor()
{
	// Factor => (Exp) | number | variable | UnaryOp Exp

	treeNode* node = new treeNode(treeNodeStmt::FACTOR_STMT);
	node->line = getNextTokenLine();

	if(checkNextToken(tokenType::LPARENT))
	{
		consumeNextToken(tokenType::LPARENT);
		node->mLeft = parseExpStmt();
		consumeNextToken(tokenType::RPARENT);
	}
	else if(checkNextToken(tokenType::INUM) || checkNextToken(tokenType::RNUM))
	{
		node->mLeft = parseNumber();
	}
	else if(checkNextToken(tokenType::IDENTIFER))
	{
		// This is different from source code
		node->mLeft = parseVar();
	}
	else if(checkNextToken(tokenType::NGT) || checkNextToken(tokenType::PST))
	{
		// Since it makes more sense UnaryOp 'belongs to' factor, no need a function to parse it
		node->mTokenType = getNextTokenType();
		consumeNextToken(getNextTokenType());
		node->mLeft = parseExpStmt();
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be factor");
	}
	
	return node;
}

treeNode* parseVar()
{
	// Variable could be an identifer or member of an array

	treeNode* node = new treeNode(treeNodeStmt::VAR_STMT);
	node->line = getNextTokenLine();

	if(checkNextToken(tokenType::IDENTIFER))
	{
		node->content = getNextToken();
		consumeNextToken(tokenType::IDENTIFER);
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be " + tokenEnum2String(tokenType::IDENTIFER));
	}

	treeNode* temp = node;
	while(checkNextToken(tokenType::LBRACKET))
	{
		if(temp->mRight != NULL)
		{
			temp->mNext = new treeNode();
			temp = temp->mNext;
		}

		consumeNextToken(tokenType::LBRACKET);
		temp->mRight = parseExpStmt();
		consumeNextToken(tokenType::RBRACKET);
	}

	return node;
}

treeNode* parseNumber()
{
	treeNode* node = new treeNode(treeNodeStmt::NUMBER_STMT);
	node->line = getNextTokenLine();

	if(checkNextToken(tokenType::INUM) || checkNextToken(tokenType::RNUM))
	{
		tokenType type = getNextTokenType();
		node->mTokenType = type;
		node->content = getNextToken();
		consumeNextToken(type);
		return node;
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be a number");
	}

	return node;
}

treeNode* parseLogicalOp()
{
	treeNode* node = new treeNode(treeNodeStmt::OP_STMT);
	node->line = getNextTokenLine();

	if (checkNextToken(tokenType::GT) || checkNextToken(tokenType::LT) || checkNextToken(tokenType::EQ) || checkNextToken(tokenType::NEQ))
	{
		tokenType type = getNextTokenType();
		node->mTokenType = type;
		consumeNextToken(type);
		return node;
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be a logical operator");
	}

	return node;
}

treeNode* parseMultiplyOp()
{
	treeNode* node = new treeNode(treeNodeStmt::OP_STMT);
	node->line = getNextTokenLine();

	if (checkNextToken(tokenType::MULT) || checkNextToken(tokenType::DIV))
	{
		tokenType type = getNextTokenType();
		node->mTokenType = type;
		consumeNextToken(type);
		return node;
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be a multiply operator");
	}

	return node;
}

treeNode* parseAdditiveOp()
{
	treeNode* node = new treeNode(treeNodeStmt::OP_STMT);
	node->line = getNextTokenLine();

	if (checkNextToken(tokenType::PLUS) || checkNextToken(tokenType::MINUS))
	{
		tokenType type = getNextTokenType();
		node->mTokenType = type;
		consumeNextToken(type);
		return node;
	}
	else
	{
		throw std::runtime_error("At line" + std::to_string(getNextTokenLine()) + ": next token should be an additive operator");
	}

	return node;
}

bool syntaxAnalysis(std::vector<std::string>& inputs,std::vector<treeNode*>& outputs)
{
	// Don't forget that the inputs goes like: token1, tokenType1, line1, token2 ......

	inputsLocal = &inputs;
	outputsLocal = &outputs;

	bool hasError = false;

ANALYSIS:
	try
	{	
		while (next < inputsLocal->size() - 2)
		{
			outputsLocal->push_back(parseStmt());
		}
	}
	catch (std::exception e)
	{
		std::string errorStr = e.what();
		std::cerr << e.what() << std::endl;
		
		hasError = true;

		// Jump through errors until ';'
		while(next < inputsLocal->size() - 2 && getNextToken() != ";")
			consumeNextToken(getNextTokenType());
		
		if (next < inputsLocal->size() - 2)
		{
			// Consume ';' and continue analyse
			consumeNextToken(getNextTokenType());
			goto ANALYSIS;
		}		
		else
		{
			return hasError;
		}		
	}

	// Unclose site error
//if(uncloseParent)
//{
//	while (next < inputsLocal->size() - 2 && getNextToken() != ")")
//	{
//		consumeNextToken(getNextTokenType());
//	}		

//	if (next < inputsLocal->size() - 2)
//	{
//		consumeNextToken(getNextTokenType());
//		goto WHILE;
//	}				
//	else
//		return false;
//}
//else if(uncloseBrasket)
//{
//	while (next < inputsLocal->size() - 2 && getNextToken() != "]")
//	{
//		consumeNextToken(getNextTokenType());
//	}

//	if (next < inputsLocal->size() - 2)
//	{
//		consumeNextToken(getNextTokenType());
//		goto WHILE;
//	}
//	else
//		return false;
//}
//else if(uncloseBrace)
//{
//	while (next < inputsLocal->size() - 2 && getNextToken() != "]")
//	{
//		consumeNextToken(getNextTokenType());
//	}

//	if (next < inputsLocal->size() - 2)
//	{
//		consumeNextToken(getNextTokenType());
//		goto WHILE;
//	}
//	else
//		return false;
//}

	return hasError;
}

std::string treeNodeEnum2String(treeNodeStmt stmt)
{
	std::string str;
	switch (stmt)
	{
		case IF_STMT: str = "If Statement"; break;
		case WHILE_STMT:str = "While Statement"; break;
		case READ_STMT:str = "Read Statement"; break;
		case WRITE_STMT:str = "Write Statement"; break;
		case DECLARE_STMT:str = "Declare Statement"; break;
		case ASSIGN_STMT:str = "Assign Statement"; break;
		case BLOCK_STMT:str = "Left Brace Statement"; break;
		case ADDITIVE_EXP_STMT:str = "Additive Expression Statement"; break;
		case LOGICAL_EXP_STMT:str = "Logical Expression Statement"; break;
		case VAR_STMT:str = "Variable Statement"; break;
		case OP_STMT:str = "Operator Statement"; break;
		case TERM_STMT:str = "Term Statement"; break;
		case FACTOR_STMT:str = "Factor Statement"; break;
		case NUMBER_STMT:str = "Number Statement"; break;
		case NOTYET_STMT:str = "NotYet Statement"; break;
		default:
			str = "Wrong String !!!!";
	}
	return str;
}