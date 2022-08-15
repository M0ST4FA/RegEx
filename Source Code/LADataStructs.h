#pragma once

#include <string>

namespace m0st4fa {

	// FLAGS
	enum struct LA_FLAG {
		LAF_NONE                    = 0b0000,
		LAF_ALLOW_WHITE_SPACE_CHARS = 0b0001,
		LAF_ALLOW_NEW_LINE          = 0b0010,
		LAF_MAX_NUM = 3,
	};


	// TOKEN
	template <typename TerminalT>
		requires requires { TerminalT::T_EOF; TerminalT::T_EPSILON; }
	struct Token {
		TerminalT name = TerminalT::T_EOF;
		std::string lexeme = "\0";

		static Token<TerminalT> EPSILON;

		bool operator==(const Token& other) const {
			return name == other.name && lexeme == other.lexeme;
		};
	};

	template <typename TerminalT>
		requires requires { TerminalT::T_EOF; TerminalT::T_EPSILON; }
	Token<TerminalT> Token<TerminalT>::EPSILON = { TerminalT::T_EPSILON, "\0" };


	template <typename TerminalT>
	std::ostream& operator<<(std::ostream& os, const m0st4fa::Token<TerminalT>& token) {
		os << "<" << token.name << ", " << token.lexeme << ">";
		return os;
	}
	

	template<typename TokenT, typename InputT = std::string>
	//                               state    lexeme
	using TokenFactoryT = TokenT(*)(state_t, InputT);
	
}

namespace {

	
	
	
}