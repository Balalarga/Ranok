#include "Lexer.h"

#include <cctype>

namespace Ranok
{
const std::map<char, Token::Type> Lexer::SymbolTypes
{
		{ '\0', Token::Type::EndFile },
		{ '\n', Token::Type::NewLine },
		{ '=', Token::Type::Assign },
		{ ',', Token::Type::Comma },
		{ '.', Token::Type::Dot },
		{ '^', Token::Type::Hat },
		{ '*', Token::Type::Star },
		{ '+', Token::Type::Plus },
		{ '-', Token::Type::Minus },
		{ '&', Token::Type::Ampersand },
		{ '|', Token::Type::Pipe },
		{ '/', Token::Type::Slash },
		{ '\\', Token::Type::BackSlash },
		{ '(', Token::Type::ParenthesisOpen },
		{ ')', Token::Type::ParenthesisClose },
		{ '[', Token::Type::BracketOpen },
		{ ']', Token::Type::BracketClose },
		{ '{', Token::Type::BraceOpen },
		{ '}', Token::Type::BraceClose },
		{ ':', Token::Type::Colon },
		{ ';', Token::Type::Semicolon },
	};


Lexer::Lexer(std::string_view code):
	_basicText(code)
{
	FillQueue(_basicText);
}

Token Lexer::Take()
{
	Token val = _lexemes.front();
	_lexemes.pop();
	return val;
}

void Lexer::FillQueue(std::string_view code)
{
	int lineCounter = 1;
	int columnCounter = 1;
	while (!code.empty())
	{
		if (isspace(code[0]))
		{
			if (code[0] == '\t')
			{
				columnCounter += 4;
			}
			else if (code[0] == '\n')
			{
				++lineCounter;
				columnCounter = 1;
			}
			else
			{
				++columnCounter;
			}
			code = code.substr(1);
			continue;
		}

		if (code.starts_with("//"))
		{
			code = code.substr(code.find_first_of('\n'));
			continue;
		}

		if (isalpha(code[0]))
		{
			int wordLen = 0;
			while (isalpha(code[++wordLen]) || isdigit(code[wordLen]) || code[wordLen] == '_');
			
			_lexemes.push({Token::Type::Word, std::string(code.substr(0, wordLen)), lineCounter, columnCounter});
			columnCounter += wordLen;
			code = code.substr(wordLen);
			continue;
		}
		
		auto symbolIter = SymbolTypes.find(code[0]);
		if (symbolIter != SymbolTypes.end())
		{
			_lexemes.push({symbolIter->second, std::string(1, code[0]), lineCounter, columnCounter});
			++columnCounter;
			code = code.substr(1);
			continue;
		}
		
		if (isdigit(code[0]))
		{
			int digitLen = 0;
			while (isdigit(code[++digitLen]));
			
			_lexemes.push({Token::Type::Number,
				std::string(code.substr(0, digitLen)),
				lineCounter,
				columnCounter
			});
			columnCounter += digitLen;
			code = code.substr(digitLen);
			continue;
		}
		
		_lexemes.push({Token::Type::Unexpected, std::string(code.substr(0, 1)), lineCounter, columnCounter});
		++columnCounter;
		code = code.substr(1);
	}
	_lexemes.push({Token::Type::EndFile, "", lineCounter, columnCounter});
}
}