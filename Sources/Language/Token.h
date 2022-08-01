#pragma once

#include <string>

namespace Ranok
{
struct Token
{
	enum class Type
	{
		EndFile,
		NewLine,
		Word,
		IntNumber,
		DoubleNumber,
		Assign,
		Comma,
		Dot,
		Hat,
		Star,
		Plus,
		Minus,
		Ampersand,
		Pipe,
		Slash,
		BackSlash,
		ParenthesisOpen,
		ParenthesisClose,
		BracketOpen,
		BracketClose,
		BraceOpen,
		BraceClose,
		Colon,
		Semicolon,
		Unexpected
	};
	Token() = default;
	Token(Type type, std::string string): type(type), string(std::move(string)) {}
	Token(Type type, std::string string, int line, int column):
		type(type), string(std::move(string)), line(line), column(column) {}
	
	Type type = Type::Unexpected;
	std::string string;
	int line{};
	int column{};

	static std::string TypeString(Type type)
	{
		switch (type)
		{
		case Type::EndFile: return "EndFile";
		case Type::NewLine: return "NewLine";
		case Type::Word: return "Word";
		case Type::IntNumber: return "IntNumber";
		case Type::DoubleNumber: return "FloatNumber";
		case Type::Assign: return "Assign";
		case Type::Comma: return "Comma";
		case Type::Dot: return "Dot";
		case Type::Hat: return "Hat";
		case Type::Star: return "Star";
		case Type::Plus: return "Plus";
		case Type::Minus: return "Minus";
		case Type::Ampersand: return "Ampersand";
		case Type::Pipe: return "Pipe";
		case Type::Slash: return "Slash";
		case Type::BackSlash: return "BackSlash";
		case Type::ParenthesisOpen: return "ParenthesisOpen";
		case Type::ParenthesisClose: return "ParenthesisClose";
		case Type::BracketOpen: return "BracketOpen";
		case Type::BracketClose: return "BracketClose";
		case Type::BraceOpen: return "BraceOpen";
		case Type::BraceClose: return "BraceClose";
		case Type::Colon: return "Colon";
		case Type::Semicolon: return "Semicolon";
		case Type::Unexpected:
		default: return "Unexpected";
		}
	}
	
	std::string TypeToString() const
	{
		return TypeString(type);
	}
};
}