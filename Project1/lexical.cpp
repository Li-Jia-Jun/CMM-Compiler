#include"lexical.h"


std::string identifers[MAX_IDENTIFERS] = {};
int identiferNum = 0;
int line = 1;

std::string tokenEnum2String(tokenType type)
{
	std::string str;

	switch (type)
	{
		case IF:		str = "IF"; break;
		case ELSE:		str = "ELSE"; break;
		case READ:		str = "READ"; break;
		case WRITE:		str = "WRITE"; break;
		case INT:		str = "INT"; break;
		case REAL:		str = "REAL"; break;
		case LPARENT:	str = "LPARENT"; break;
		case RPARENT:	str = "RPARENT"; break;
		case LBRACKET:	str = "LBRACKET"; break;
		case RBRACKET:	str = "RBRACKET"; break;
		case LBRACE:	str = "LBRACE"; break;
		case RBRACE:	str = "RBRACE"; break;
		case SEMI:		str = "SEMICOLON"; break;
		case COMMA:		str = "COMMA"; break;
		case PLUS:		str = "PLUS"; break;
		case MINUS:		str = "MINUS"; break;
		case MULT:		str = "MULT"; break;
		case DIV:		str = "DIV"; break;
		case ASSIGN:	str = "ASSIGN"; break;
		case GT:		str = "GREATER THAN"; break;
		case LT:		str = "LESS THAN"; break;
		case EQ:		str = "EQUAL"; break;
		case NEQ:		str = "NOT EQUAL"; break;
		case NGT:		str = "NEGITIVE"; break;
		case PST:		str = "POSITIVE"; break;
		case IDENTIFER:	str = "IDENTIFER"; break;
		case INUM:		str = "INUM"; break;
		case RNUM:		str = "RNUM"; break;
		case UNIDENTIFIED:	str = "UNIDENTIFIED"; break;
		case UNCLOSESITE:	str = "UNCLOSESITE"; break;
		case WNUM:		str = "WWRONGNUM"; break;
		default:		str = "SOME_ERROR_OCCURS_WHILE_GETTING_TOKEN_TYPE"; break;
	}

	return str;
}

bool isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

bool isLetter(char ch)
{
	return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}

tokenType getKeyWord(std::string token)
{
	if(token == "if") return tokenType::IF;
	else if(token == "else")	return tokenType::ELSE;
	else if(token == "read")	return tokenType::READ;
	else if (token == "write")	return tokenType::WRITE;
	else if (token == "int")	return tokenType::INT;
	else if (token == "real")	return tokenType::REAL;
	else if(token == "while")	return tokenType::WHILE;
	else						return tokenType::NONE;
}

bool getIdentifer(std::string token)
{
	for (int i = 0; i < identiferNum; i++)
	{
		if (token == identifers[i])
			return true;
	}

	// New recognized identifer
	identifers[identiferNum++] = token;

	return true;
}

bool lexicalAnalysis(std::string fileName,std::vector<std::string>& outputs)
{
	// Output format goes like: token1, tokenType1, line1, token2, tokenType2, line2, ... 

	std::ifstream inFile = std::ifstream(fileName,std::ifstream::in | std::ifstream::_Nocreate);

	if (!inFile.is_open())
	{
		std::cerr << "wrong file name!" << std::endl;
		return false;
	}

	bool hasError = false;

	char ch;
	inFile >> std::noskipws;	// Don't skip white space
	inFile >> ch;
	while (!inFile.eof())
	{
		std::string token;

		// Filtering spaces, tabs and carriage returns
		while (!inFile.eof() && (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t'))
		{
			if (ch == '\n')
				++line;

			inFile >> ch;
		}

		if (isLetter(ch) || ch == '_')				// Identifer or key word
		{
			do
			{
				token += ch;
				inFile >> ch;
			} while (isDigit(ch) || isLetter(ch) || ch == '_');

			outputs.push_back(token);

			if (getKeyWord(token) != tokenType::NONE)
				outputs.push_back(std::to_string(getKeyWord(token)));		
			else if (getIdentifer(token))
				outputs.push_back(std::to_string(tokenType::IDENTIFER));
			else
			{
				outputs.push_back(std::to_string(tokenType::UNIDENTIFIED));
				hasError = true;
			}		

			outputs.push_back(std::to_string(line));
		}
		else if (isDigit(ch))													// Non-negative digit
		{
			bool isRealNumber = false;
			do
			{
				token += ch;
				inFile >> ch;

				if (ch == '.')
					isRealNumber = true;
			} while (isDigit(ch) || ch == '.');

			outputs.push_back(token);

			if (token.size() > 2 && token.at(0) == '0' && token.at(1) != '.')// Number that starts with 0 but not a fraction
			{
				outputs.push_back(std::to_string(tokenType::WNUM));
				hasError = true;
			}			
			else if (isRealNumber)
				outputs.push_back(std::to_string(tokenType::RNUM));
			else
				outputs.push_back(std::to_string(tokenType::INUM));

			outputs.push_back(std::to_string(line));
		}
		else
		{
			if (ch == '/')	// Could be '/', '//' or '/*'
			{
				token += ch;
				inFile >> ch;

				if (ch == '/')
				{
					while (!(ch == '\n' || ch == EOF))
					{
						if (ch == '\n')
							line++;
						inFile >> ch;
					}

				}
				else if (ch == '*')
				{
					char chNext;
					inFile >> ch;
					inFile >> chNext;

					if(ch == '\n')
						line++;

					bool hasClose = false;
					int startLine = line;
					do
					{
						ch = chNext;
						chNext = inFile.get();

						if (ch == '*' && chNext == '/')
						{
							hasClose = true;
							break;
						}

						if (chNext == '\n')
							line++;

					} while (chNext != EOF);

					if (!hasClose)
					{
						outputs.push_back(COMMENT);
						outputs.push_back(std::to_string(tokenType::UNCLOSESITE));
						outputs.push_back(std::to_string(startLine));
						hasError = true;
					}

					inFile >> ch;
				}
				else
				{
					outputs.push_back(token);
					outputs.push_back(std::to_string(tokenType::DIV));
					outputs.push_back(std::to_string(line));
				}
			}
			else if (ch == '=')	// Could be '=' or '=='
			{
				token += ch;
				inFile >> ch;

				if (ch == '=')
				{
					token += ch;
					inFile >> ch;
					outputs.push_back(token);
					outputs.push_back(std::to_string(tokenType::EQ));
				}
				else
				{
					outputs.push_back(token);
					outputs.push_back(std::to_string(tokenType::ASSIGN));
				}

				outputs.push_back(std::to_string(line));
			}
			else if (ch == '<')
			{
				token += ch;
				inFile >> ch;

				if (ch == '>')
				{
					token += ch;
					inFile >> ch;
					outputs.push_back(token);
					outputs.push_back(std::to_string(tokenType::NEQ));
				}
				else
				{
					outputs.push_back(token);
					outputs.push_back(std::to_string(tokenType::LT));
				}
				outputs.push_back(std::to_string(line));
			}
			else if (ch == '-')	// Could be negative or minus
			{
				token += ch;
				inFile >> ch;

				outputs.push_back(token);

				// Minus if the last token is identifer or number		
				if (outputs.size() > 3 &&
					(outputs.at(outputs.size() - 3) == std::to_string(tokenType::IDENTIFER) ||
						outputs.at(outputs.size() - 3) == std::to_string(tokenType::INUM) ||
						outputs.at(outputs.size() - 3) == std::to_string(tokenType::RNUM) ||
						outputs.at(outputs.size() - 3) == std::to_string(tokenType::WNUM)))
				{
					outputs.push_back(std::to_string(tokenType::MINUS));
				}
				else
				{
					outputs.push_back(std::to_string(tokenType::NGT));
				}
				outputs.push_back(std::to_string(line));
			}
			else
			{
				char lastChar = ch;
				token += ch;
				inFile >> ch;
				outputs.push_back(token);

				if(lastChar == ',') outputs.push_back(std::to_string(tokenType::COMMA));
				else if (lastChar == ';') outputs.push_back(std::to_string(tokenType::SEMI));
				else if (lastChar == '(') outputs.push_back(std::to_string(tokenType::LPARENT));
				else if (lastChar == ')') outputs.push_back(std::to_string(tokenType::RPARENT));
				else if (lastChar == '[') outputs.push_back(std::to_string(tokenType::LBRACKET));
				else if (lastChar == ']') outputs.push_back(std::to_string(tokenType::RBRACKET));
				else if (lastChar == '{') outputs.push_back(std::to_string(tokenType::LBRACE));
				else if (lastChar == '}') outputs.push_back(std::to_string(tokenType::RBRACE));
				else if (lastChar == '+') outputs.push_back(std::to_string(tokenType::PLUS));
				else if (lastChar == '*') outputs.push_back(std::to_string(tokenType::MULT));
				else if (lastChar == '>') outputs.push_back(std::to_string(tokenType::GT));
				else
				{
					// Detect '\n' produced by 'inFile >>' when EOF
					if (ch == '\n' || ch == '\t')
					{
						outputs.pop_back();	// Remove the token added above
						break;
					}

					// Unidentified
					outputs.push_back(std::to_string(tokenType::UNIDENTIFIED));
					hasError = true;
				}
				outputs.push_back(std::to_string(line));
			}
		}
	}

	return hasError;
}