#pragma once
#include <queue>

#include "Token.h"


class ActionNode
{
public:
	ActionNode(const std::string& name);
	virtual ~ActionNode() = default;
	
	virtual std::queue<ActionNode*> WalkDown() const;
	
	const std::string& Name() const { return _name; }
	
	
protected:
	const std::string _name;
};


class DoubleNumberNode: public ActionNode
{
public:
	DoubleNumberNode(double number);
	
	double Value() const { return _value; }
	
	
private:
	double _value = 0.0;
};


class IntNumberNode: public ActionNode
{
public:
	IntNumberNode(int number);
	
	int Value() const { return _value; }
	
	
private:
	int _value = 0;
};


class ArrayGetterNode: public ActionNode
{
public:
	ArrayGetterNode(const std::string& name, ActionNode* id);
	
	
private:
	ActionNode* _id;
};

class ArrayDeclarationNode: public ActionNode
{
public:
	ArrayDeclarationNode(const std::string& name, std::vector<ActionNode*> values);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
	const std::vector<ActionNode*>& Values() const { return _values; }
	
	
private:
	std::vector<ActionNode*> _values;
};


class VariableDeclarationNode: public ActionNode
{
public:
	VariableDeclarationNode(const std::string& name, ActionNode* value);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
private:
	ActionNode* _value;
};


class VariableNode: public ActionNode
{
public:
	VariableNode(const std::string& name, VariableDeclarationNode* decl);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
private:
	VariableDeclarationNode* _declaration;
};


class UnaryNode: public ActionNode
{
public:
	UnaryNode(const std::string& name, ActionNode* child);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Child() const { return _child; }
	
	
private:
	ActionNode* _child;
};


class BinaryNode: public ActionNode
{
public:
	BinaryNode(const std::string& name, ActionNode* left, ActionNode* right);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Left() const { return _left; }
	ActionNode* Right() const { return _right; }
	
	
private:
	ActionNode* _left, *_right;
};


class FunctionCallNode: public ActionNode
{
public:
	FunctionCallNode(const std::string& name, std::vector<ActionNode*> arguments);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	const std::vector<ActionNode*>& Arguments() const { return _arguments; }
	
	
private:
	std::vector<ActionNode*> _arguments;
};


class FunctionSignature
{
public:
	FunctionSignature(const std::string& name, const std::vector<std::string>& args);

	const std::string& Name() const { return _name; }
	const std::vector<std::string>& Args() const { return _arguments; }
	
	
private:
	std::string _name;
	std::vector<std::string> _arguments;
};

class FunctionDeclarationNode: public ActionNode
{
public:
	FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body);
	
	
private:
	FunctionSignature _signature;
	ActionNode* _body;
};