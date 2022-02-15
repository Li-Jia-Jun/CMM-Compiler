#include"Util.h"
#include"interpret.h"
using namespace std;

int main()
{
	//---------------Lexical Analysis Test-----------------------------------------
	//std::vector<std::string> outputs;
	//
	//lexicalAnalysis("test.cmm", outputs);
	//for (int i = 0; i < outputs.size(); i += 3)
	//{
	//	std::cout << outputs.at(i) << ",		" << tokenEnum2String((tokenType)std::atoi(outputs.at(i + 1).c_str())) << ",	at line "<<outputs.at(i+2)<<std::endl;
	//}
	//------------------------------------------------------------------------------


	//////-------------------Parse Test--------------------------------------------------
	//vector<treeNode*> outputs;
	//vector<string> inputs;
	//bool hasError = lexicalAnalysis("test.cmm",inputs);
	//if(hasError == false)
	//{
	//	syntaxAnalysis(inputs, outputs);
	//	cout<<"\n\nÓï·¨Ê÷ÈçÏÂ£º"<< endl;
	//	printSyntaxTree(outputs);
	//}
	//else
	//{
	//	cout<<"Error(s) detected in lexical analysis, please correct error(s) to continue syntax analysis!"<< endl;
	//}
	//////----------------------------------------------------------------------------------



	//-------------------Interpretation Test--------------------------------------------------
//	vector<treeNode*> treeNodes;
//	vector<string> tokens;
//	bool hasError = lexicalAnalysis("test.cmm", tokens);
//	if (hasError == false)
//	{
//		hasError = syntaxAnalysis(tokens, treeNodes);
//		if(hasError == false)
//		{
//			interpret(treeNodes);
//		}
//		else
//		{
//			cout << "Error(s) detected in during syntax analysis, please correct error(s) to continue to interpretation!" << endl;
//		}
//	}
//	else
//	{
//		cout << "Error(s) detected in during lexical analysis, please correct error(s) to continue to interpretation!" << endl;
//	}
	//----------------------------------------------------------------------------------

	system("pause");
	return 0;
}

