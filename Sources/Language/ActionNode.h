#pragma once
#include <map>
#include <memory>
#include <queue>

#include "Token.h"

namespace Ranok
{
class FunctionSignature;
class FunctionDeclarationNode;
class ActionNode;
class VariableDeclarationNode;


class ActionNodeFactory
{
public:
	template<class T>
	struct Commitable
	{
		Commitable(const std::string& name, T* val, std::map<std::string, T*>& container):
			_name(name),
			_val(val),
			_parentContainer(container)
		{
		}
		
		~Commitable()
		{
			if (bCommit)
			{
				auto it = _parentContainer.find(_name);
				if (it == _parentContainer.end())
					_parentContainer.insert({_name, _val});
				else
					delete _val;
			}
		}
		T* Commit() { bCommit = true; return _val; }
		T* Get() { return _val; }
		
	private:
		bool bCommit = false;
		std::string _name;
		T* _val;
		std::map<std::string, T*>& _parentContainer;
	};
	
	
	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ActionNode>, T*> Create(TArgs&& ...args)
	{
		_nodes.push_back(std::make_unique<T>(args...));
		return static_cast<T*>(_nodes.back().get());
	}
	
	ActionNodeFactory operator+(const ActionNodeFactory& oth);
	
	std::shared_ptr<Commitable<FunctionDeclarationNode>> TempCreateFunction(const FunctionSignature& signature);
	
	VariableDeclarationNode* CreateVariable(const std::string& name, ActionNode* value = nullptr);
	FunctionDeclarationNode* CreateFunction(const FunctionSignature& signature, ActionNode* body = nullptr);
	
	VariableDeclarationNode* FindVariable(const std::string& name) const;
	FunctionDeclarationNode* FindFunction(const std::string& name) const;
	
	std::vector<std::unique_ptr<ActionNode>>& Nodes() { return _nodes; }
	std::map<std::string, FunctionDeclarationNode*>& Functions() { return _functions; }
	std::map<std::string, VariableDeclarationNode*>& Variables() { return _variables; }
	
	
private:
	std::vector<std::unique_ptr<ActionNode>> _nodes;
	std::map<std::string, FunctionDeclarationNode*> _functions;
	std::map<std::string, VariableDeclarationNode*> _variables;
};

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
	VariableNode(VariableDeclarationNode* decl);
	
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
	FunctionCallNode(FunctionDeclarationNode* root, std::vector<ActionNode*> arguments);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	const std::vector<ActionNode*>& Arguments() const { return _arguments; }
	
	FunctionDeclarationNode* Root() const { return _root; }
	FunctionDeclarationNode* Root() { return _root; } 
	
	
private:
	FunctionDeclarationNode* _root;
	std::vector<ActionNode*> _arguments;
};


class FunctionSignature
{
public:
	FunctionSignature(const std::string& name, const std::vector<ActionNode*>& args = {});
	
	const std::string& Name() const { return _name; }
	
	std::vector<ActionNode*>& Args() { return _arguments; }
	const std::vector<ActionNode*>& Args() const { return _arguments; }
	
	
private:
	std::string _name;
	std::vector<ActionNode*> _arguments;
};

class FunctionDeclarationNode: public ActionNode
{
public:
	FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNodeFactory& Factory() { return _factory; }
	const ActionNodeFactory& Factory() const { return _factory; }
	
	FunctionSignature& Signature() { return _signature; }
	const FunctionSignature& Signature() const { return _signature; }
	
	ActionNode* Body() { return _body; }
	void SetBody(ActionNode* body) { _body = body; }
	
	
private:
	FunctionSignature _signature;
	ActionNode* _body;
	ActionNodeFactory _factory;
};
}