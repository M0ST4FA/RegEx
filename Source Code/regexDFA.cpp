#pragma once

#include "regex.h"

namespace m0st4fa {
	namespace regex {
		
		DFATableType RegularExpression::_get_dfa_table()
		{
			DFATableType table{};

			/* Final States:
				1. |
				2. ^
				3. $
				4. *
				5. *?
				6. +
				7. +?
				8. ?
				9. ??
				10. (
				11. )
				12. [
				13. [^
				14. ]
				15. {
				16. }
				17. \
				18. alpha
				19. number
			*/

			// |
			table[1]['|'] = 2;

			// ^
			table[1]['^'] = 3;

			// $
			table[1]['$'] = 4;

			// * and *?
			table[1]['*'] = 5;
			table[4]['?'] = 6;

			// + and +?
			table[1]['+'] = 7;
			table[6]['?'] = 8;

			// ? and ??
			table[1]['?'] = 9;
			table[8]['?'] = 10;

			// ( and )
			table[1]['('] = 11;
			table[1][')'] = 12;

			// [ and ]
			table[1]['['] = 13;
			table[13]['^'] = 14;
			table[1][']'] = 15;

			// { and }
			table[1]['{'] = 16;
			table[1]['}'] = 17;

			// back slash
			table[1]['\\'] = 18;

			// alphanumber
			for (char c = 'a'; c <= 'z'; c++)
				table[1][c] = 19;
			for (char c = 'A'; c <= 'Z'; c++)
				table[1][c] = 19;
			table[1][' '] = 19;

			for (char c = '0'; c <= '9'; c++)
				table[1][c] = 20;

			table[1][REGEX_END_MARKER] = 19;

			return table;
		}

		TokenType RegularExpression::_token_factory(state_t state, [[maybe_unused]] InputType lexeme)
		{
			TokenType token;

			switch (state) {
				using enum m0st4fa::Terminal;
			case 2:
				token.name = T_OP_OR;
				break;
			case 3:
				token.name = T_ANC_CHARET;
				break;
			case 4:
				token.name = T_ANC_DOLLAR;
				break;
			case 5:
				token.name = T_QUANT_STAR_GREEDY;
				break;
			case 6:
				token.name = T_QUANT_STAR_LAZY;
				break;
			case 7:
				token.name = T_QUANT_PLUS_GREEDY;
				break;
			case 8:
				token.name = T_QUANT_PLUS_LAZY;
				break;
			case 9:
				token.name = T_QUANT_QMARK_GREEDY;
				break;
			case 10:
				token.name = T_QUANT_QMARK_LAZY;
				break;
			case 11:
				token.name = T_LEFT_PAREN;
				break;
			case 12:
				token.name = T_RIGHT_PAREN;
				break;
			case 13:
				token.name = T_LEFT_BRACKET;
				break;
			case 14:
				token.name = T_LEFT_BRACKET_CHARET;
				break;
			case 15:
				token.name = T_RIGHT_BRACKET;
				break;
			case 16:
				token.name = T_LEFT_BRACE;
				break;
			case 17:
				token.name = T_RIGHT_BRACE;
				break;
			case 18:
				token.name = T_BACK_SLASH;
				break;
			case 19:
				token.name = T_ALPHA;
				token.attribute = lexeme;
				break;
			case 20:
				token.name = T_NUM;
				token.attribute = lexeme;
				break;

			default:
				throw std::exception("AT FUNCTION _token_factory(), UNIDENTIFIED STATE. THIS IS TEMPORARY HANDLING!");
			}

			return token;
		}
	}
}