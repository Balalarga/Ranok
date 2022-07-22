#pragma once
#include <queue>
#include <string>

#include "Token.h"


class ActionNode
{
public:
	ActionNode(Token token);
	virtual ~ActionNode() = default;
	
	virtual std::queue<ActionNode*> WalkDown();
	
	const Token& GetToken() const { return _token; }
	
private:
	Token _token;
};


class DoubleNumberNode: public ActionNode
{
public:
	DoubleNumberNode(Token token, double value);
	
	double Value() const { return _value; }
	
	
private:
	double _value;
};


class IntNumberNode: public ActionNode
{
public:
	IntNumberNode(Token token, int value);
	
	int Value() const { return _value; }
	
	
private:
	int _value;
};


class ArrayNode: public ActionNode
{
public:
	ArrayNode(Token token, std::vector<ActionNode*> values);
	
	const std::vector<ActionNode*>& Value() const { return _values; }
	
	
private:
	std::vector<ActionNode*> _values;
};


class UnaryNode: public ActionNode
{
public:
	UnaryNode(Token token, ActionNode* child);
	
	std::queue<ActionNode*> WalkDown() override;
	
	ActionNode* Child() const { return _child; }
	
	
private:
	ActionNode* _child;
};


class BinaryNode: public ActionNode
{
public:
	BinaryNode(Token token, ActionNode* left, ActionNode* right);
	
	std::queue<ActionNode*> WalkDown() override;
	
	ActionNode* Left() const { return _left; }
	ActionNode* Right() const { return _right; }
	
	
private:
	ActionNode* _left, *_right;
};


class FunctionNode: public ActionNode
{
public:
	FunctionNode(Token token, std::vector<ActionNode*> arguments, ActionNode* result);

	std::queue<ActionNode*> WalkDown() override;
	
	const std::vector<ActionNode*>& Arguments() const { return _arguments; }
	ActionNode* Result() const { return _result; }
	
	
private:
	std::vector<ActionNode*> _arguments;
	ActionNode* _result;
};
