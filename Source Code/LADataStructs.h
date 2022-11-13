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
	template <typename TerminalT, typename AttrT = std::string>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	struct Token {
		TerminalT name = TerminalT::T_EOF;
		AttrT attribute;

		static const Token<TerminalT> EPSILON;
		static const Token<TerminalT> TEOF;

		bool operator==(const Token& other) const {
			return name == other.name && attribute == other.attribute;
		};
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {
			return "<" + stringfy(this->name) + ", " + this->attribute + ">";
		}
	};

	template <typename TerminalT, typename AttrT>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	const Token<TerminalT> Token<TerminalT, typename AttrT>::EPSILON = { TerminalT::T_EPSILON, "\0" };
	template <typename TerminalT, typename AttrT>
		requires requires (TerminalT a) { TerminalT::T_EOF; TerminalT::T_EPSILON; stringfy(a); }
	const Token<TerminalT> Token<TerminalT, typename AttrT>::TEOF = { TerminalT::T_EOF, "\0" };


	template <typename TerminalT>
		requires requires (TerminalT a) { stringfy(a); }
	std::ostream& operator<<(std::ostream& os, const m0st4fa::Token<TerminalT>& token) {
		return os << token.toString();
	}
	

	template<typename TokenT, typename InputT = std::string>
	//                               state    lexeme
	using TokenFactoryT = TokenT(*)(state_t, InputT);
	
}
