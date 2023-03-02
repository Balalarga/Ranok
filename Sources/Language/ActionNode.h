#pragma once
#include <functional>
#include <map>
#include <memory>
#include <queue>

#include "Token.h"

namespace Ranok
{
class ArrayNode;
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
		Commitable(const std::string& name, T* val, std::function<bool(const std::string&, T*)> wantDelete):
			_wantDelete(std::move(wantDelete)),
			_name(name),
			_val(val)
		{
		}
		
		~Commitable()
		{
			if (!bCommit || _wantDelete(_name, _val))
				delete _val;
		}
		T* Commit() { bCommit = true; return _val; }
		T* Get() { return _val; }
		
	private:
		bool bCommit = false;
		std::function<bool(const std::string&, T*)> _wantDelete;
		std::string _name;
		T* _val;
	};
	
	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ActionNode>, T*> Create(TArgs&& ...args)
	{
		_nodes.push_back(std::make_shared<T>(args...));
		return static_cast<T*>(_nodes.back().get());
	}
	
	std::shared_ptr<Commitable<FunctionDeclarationNode>> TempCreateFunction(const FunctionSignature& signature);
	
	VariableDeclarationNode* CreateVariable(const Token& name, ActionNode* value = nullptr);
	FunctionDeclarationNode* CreateFunction(const FunctionSignature& signature, ActionNode* body = nullptr);
	
	ArrayNode* FindArrayVariable(const std::string& name);
	VariableDeclarationNode* FindVariable(const std::string& name) const;
	FunctionDeclarationNode* FindFunction(const std::string& name) const;
	
	std::vector<std::shared_ptr<ActionNode>>& Nodes() { return _nodes; }
	const std::vector<std::shared_ptr<ActionNode>>& Nodes() const { return _nodes; }
	
	std::map<std::string, FunctionDeclarationNode*>& Functions() { return _functions; }
	const std::map<std::string, FunctionDeclarationNode*>& Functions() const { return _functions; }
	
	std::map<std::string, VariableDeclarationNode*>& Variables() { return _variables; }
	const std::map<std::string, VariableDeclarationNode*>& Variables() const { return _variables; }
	
	std::vector<ActionNode*>& DeclarationOrder() { return _declarationOrder; }
	const std::vector<ActionNode*>& DeclarationOrder() const { return _declarationOrder; }

	friend ActionNodeFactory operator+(const ActionNodeFactory& lhs, const ActionNodeFactory& rhs);
	friend ActionNodeFactory& operator+=(ActionNodeFactory& lhs, const ActionNodeFactory& rhs);
	
private:
	std::vector<std::shared_ptr<ActionNode>> _nodes;
	std::map<std::string, FunctionDeclarationNode*> _functions;
	std::map<std::string, VariableDeclarationNode*> _variables;
	std::vector<ActionNode*> _declarationOrder;
};

class ActionNode
{
public:
	/// Utility functions
	static const ArrayNode* IsArray(const ActionNode* node);
	static size_t GetArraySize(const ActionNode* node);
	
	ActionNode(const Token& token);
	virtual ~ActionNode() = default;
	
	virtual std::queue<ActionNode*> WalkDown() const;
	
	const std::string& Name() const { return _token.string; }
	const Token& GetToken() const { return _token; }
	
protected:
	const Token _token;
};

class DoubleNumberNode: public ActionNode
{
public:
	DoubleNumberNode(const Token& token, double number);
	
	double Value() const { return _value; }
	
	
private:
	double _value = 0.0;
};

class ArrayNode: public ActionNode
{
public:
	ArrayNode(const Token& token, const std::vector<ActionNode*>& values);

	virtual std::queue<ActionNode*> WalkDown() const override;
	
	const std::vector<ActionNode*>& Values() const { return _values; }
	
	
private:
	std::vector<ActionNode*> _values;
};

enum class VariableType
{
	Double, Array
};

class VariableDeclarationNode: public ActionNode
{
public:
	VariableDeclarationNode(const Token& token, ActionNode* value);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
	const ActionNode* Value() const { return _value; }
	ActionNode* Value() { return _value; }
	
private:
	ActionNode* _value;
};

class ArrayGetterNode: public ActionNode
{
public:
	ArrayGetterNode(VariableDeclarationNode* var, ActionNode* id);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Id() { return _id; }
	const ActionNode* Id() const { return _id; }
	
	VariableDeclarationNode* Var() { return _var; }
	const VariableDeclarationNode* Var() const { return _var; }
	
	
private:
	VariableDeclarationNode* _var;
	ActionNode* _id;
};


class VariableNode: public ActionNode
{
public:
	VariableNode(VariableDeclarationNode* decl);
	
	virtual std::queue<ActionNode*> WalkDown() const override;
	
	VariableDeclarationNode* Declaration() { return _declaration; }
	const VariableDeclarationNode* Declaration() const { return _declaration; }
	
private:
	VariableDeclarationNode* _declaration;
};


class UnaryNode: public ActionNode
{
public:
	UnaryNode(const Token& token, ActionNode* child);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Child() const { return _child; }
	
	
private:
	ActionNode* _child;
};


class BinaryNode: public ActionNode
{
public:
	BinaryNode(const Token& token, ActionNode* left, ActionNode* right);
	
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
	FunctionSignature(const Token& token, const std::vector<VariableDeclarationNode*>& args = {});

	const Token& GetToken() const { return _token; }
	
	std::vector<VariableDeclarationNode*>& Args() { return _arguments; }
	const std::vector<VariableDeclarationNode*>& Args() const { return _arguments; }
	
	
private:
	Token _token;
	std::vector<VariableDeclarationNode*> _arguments;
};

class FunctionDeclarationNode: public ActionNode
{
public:
	FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body);

	static std::string GetDescription(FunctionDeclarationNode* func);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNodeFactory& Factory() { return _factory; }
	const ActionNodeFactory& Factory() const { return _factory; }
	
	FunctionSignature& Signature() { return _signature; }
	const FunctionSignature& Signature() const { return _signature; }

	ActionNode* Body() { return _body; }
	const ActionNode* Body() const { return _body; }
	void SetBody(ActionNode* body) { _body = body; }
	
	
private:
	FunctionSignature _signature;
	ActionNode* _body;
	ActionNodeFactory _factory;
};

}
