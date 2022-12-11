#pragma once

namespace m0st4fa {
	enum class Terminal {
		T_OP_OR,
		T_ANC_CHARET,
		T_ANC_DOLLAR,
		T_ANC_A,
		T_ANC_z,
		T_ANC_Z,
		T_ANC_G,
		T_QUANT_STAR_GREEDY,
		T_QUANT_STAR_LAZY,
		T_QUANT_PLUS_GREEDY,
		T_QUANT_PLUS_LAZY,
		T_QUANT_QMARK_GREEDY,
		T_QUANT_QMARK_LAZY,
		T_QUANT_NTIMES_GREEDY,
		T_QUANT_NTIMES_LAZY,
		T_QUANT_NT_OR_MORE_GREEDY,
		T_QUANT_NT_OR_MORE_LAZY,
		T_QUANT_NT_OR_MT_GREEDY,
		T_QUANT_NT_OR_MT_LAZY,
		T_LEFT_PAREN,
		T_RIGHT_PAREN,
		T_LEFT_BRACKET,
		T_RIGHT_BRACKET,
		T_RIGHT_BRACKET_CHARET,
		T_LEFT_BRACE,
		T_RIGHT_BRACE,
		T_RIGHT_BRACE_LAZY,
		T_BACK_SLASH,
		T_ALPHANUM,
		T_EPSILON,
		T_EOF,
		T_COUNT
	};

	enum class Variable {
		V_REGEX,
		V_REGEX_P,
		V_ANCHOR,
		V_CONC,
		V_QUANT,
		V_GROUP,
		V_CHAR_CLASS,
		V_CHAR_CLASS_SEQ,
		V_RANGE,
		V_ESQ_SEQ,
		V_FACTOR,
		NT_COUNT
	};

	enum class Flag {
		F_NONE,
		F_COUNT,
	};

	std::string toString(const Terminal);
	// LEGACY
	std::string stringfy(const Terminal);
	std::string toString(const Variable);
	// LEGACY
	std::string stringfy(const Variable);

}