#pragma once


struct Token
{
	enum class Type
	{
		EndFile,
		NewLine,
		Word,
		Number,
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
		OpenParenthesis,
		CloseParenthesis,
		BracketOpen,
		BracketClose,
		BraceOpen,
		BraceClose,
		Colon,
		Semicolon,
		Unexpected
	};
	Type type;
	std::string string;
	int line{};
	int column{};

	std::string TypeToString() const
	{
		switch (type)
		{
			case Type::EndFile: return "EndFile";
			case Type::NewLine: return "NewLine";
			case Type::Word: return "Word";
			case Type::Number: return "Number";
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
			case Type::OpenParenthesis: return "OpenParenthesis";
			case Type::CloseParenthesis: return "CloseParenthesis";
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
};
