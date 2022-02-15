#pragma once
#include"lexical.h"


enum treeNodeStmt
{
	IF_STMT, WHILE_STMT, BREAK_STMT,						// Control statement
	READ_STMT, WRITE_STMT,			
	DECLARE_STMT,					
	ASSIGN_STMT,					
	BLOCK_STMT,												// New block statement	
	ADDITIVE_EXP_STMT, LOGICAL_EXP_STMT,					// Note that term expression is an exception 
															// or additive expression
	VAR_STMT,												// Variable					
	OP_STMT,												// Operator
	TERM_STMT, FACTOR_STMT,			
	NUMBER_STMT,
	NOTYET_STMT,
};

struct treeNode
{
	treeNodeStmt stmt;

	/*
		This is only for block statement
	*/
	treeNode* mNext;

	/*
		if statement£º				left for exp£»middle for if branch TreeNode£»right for else branch TreeNode if exist
		while statement£º			left for exp£»middle for code block
		read statement£º				left for var
		write statement£º			left for exp
		declare statement£º			two cases: (1) For array, left for var, right for leftest demension, mNext->right 
													for second leftest demension, etc
												(2) For var or varlist, left for first var, middle for assign, mNext for next var
		assign statement£º			left for var£¬middle for exp
		expression statement£º		two cases: (1)additiveExp logicalExp addtiveExp, in left middle right
											   (2)additiveExp, in left
		array variable				right for leftest demension, mNext->right for second leftest demension, etc
	*/
	treeNode* mLeft;
	treeNode* mMiddle;
	treeNode* mRight;

	std::string content;
	tokenType mTokenType;
	int line;

	treeNode():treeNode(treeNodeStmt::NOTYET_STMT)
	{
	}
	treeNode(treeNodeStmt stmt)
	{
		this->stmt = stmt;
		mLeft = NULL;
		mMiddle = NULL;
		mRight = NULL;
		mNext = NULL;
		mTokenType = tokenType::NONE;
		content = "";
	}
}; 

bool syntaxAnalysis(std::vector<std::string>& inputs,std::vector<treeNode*>& outputs);
std::string treeNodeEnum2String(treeNodeStmt stmt);
