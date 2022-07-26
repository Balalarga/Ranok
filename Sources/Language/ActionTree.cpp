#include "ActionTree.h"


ActionTree::ActionTree()
{
	InitGlobalConstants();
}

void ActionTree::InitGlobalConstants()
{
	Add<UnaryNode>(Token{Token::Type::Word, "PI"},
		Add<DoubleNumberNode>(Token(Token::Type::Number, "3.141592")));
}
