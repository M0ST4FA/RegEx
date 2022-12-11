#pragma once

#include "regexTypedefs.h"
#include "regex.h"

namespace m0st4fa {

	namespace regex {

		GrammarType RegularExpression::_get_grammar() {

			GrammarType grammar;
			size_t index = 0;

			ProductionType prod;
			SymbolType vRegEx = toSymbol(Variable::V_REGEX);
			SymbolType vRegExp = toSymbol(Variable::V_REGEX_P);
			SymbolType vConc = toSymbol(Variable::V_CONC);
			SymbolType vQuant = toSymbol(Variable::V_QUANT);
			SymbolType vCharClass = toSymbol(Variable::V_CHAR_CLASS);
			SymbolType vCharClassSeq = toSymbol(Variable::V_CHAR_CLASS_SEQ);
			SymbolType vAnch = toSymbol(Variable::V_ANCHOR);
			SymbolType vEscapeSeq = toSymbol(Variable::V_ESQ_SEQ);
			SymbolType vGroup = toSymbol(Variable::V_GROUP);
			SymbolType vRange = toSymbol(Variable::V_RANGE);
			SymbolType vFact = toSymbol(Variable::V_FACTOR);

			SymbolType tOr = toSymbol(Terminal::T_OP_OR);
			SymbolType tAnchCharet = toSymbol(Terminal::T_ANC_CHARET);
			SymbolType tAnchDollar = toSymbol(Terminal::T_ANC_DOLLAR);
			SymbolType tAncA = toSymbol(Terminal::T_ANC_A);
			SymbolType tAncz = toSymbol(Terminal::T_ANC_z);
			SymbolType tAncZ = toSymbol(Terminal::T_ANC_Z);
			SymbolType tAncG = toSymbol(Terminal::T_ANC_G);
			SymbolType tBackSlash = toSymbol(Terminal::T_BACK_SLASH);
			SymbolType tEOF = toSymbol(Terminal::T_EOF);
			SymbolType tEpsilon = toSymbol(Terminal::T_EPSILON);
			SymbolType tLBracket = toSymbol(Terminal::T_LEFT_BRACKET);
			SymbolType tLParen = toSymbol(Terminal::T_LEFT_PAREN);
			SymbolType tLBrace = toSymbol(Terminal::T_LEFT_BRACE);
			SymbolType tRBracket = toSymbol(Terminal::T_RIGHT_BRACKET);
			SymbolType tRBracketCharet = toSymbol(Terminal::T_RIGHT_BRACKET_CHARET);
			SymbolType tRParen = toSymbol(Terminal::T_RIGHT_PAREN);
			SymbolType tRBrace = toSymbol(Terminal::T_RIGHT_BRACE);
			SymbolType tRBraceLZ = toSymbol(Terminal::T_RIGHT_BRACE_LAZY);
			SymbolType tQNTG = toSymbol(Terminal::T_QUANT_NTIMES_GREEDY);
			SymbolType tQNTLZ = toSymbol(Terminal::T_QUANT_NTIMES_LAZY);
			SymbolType tQNTMG = toSymbol(Terminal::T_QUANT_NT_OR_MORE_GREEDY);
			SymbolType tQNTMLZ = toSymbol(Terminal::T_QUANT_NT_OR_MORE_LAZY);
			SymbolType tQNTMTG = toSymbol(Terminal::T_QUANT_NT_OR_MT_GREEDY);
			SymbolType tQNTMTLZ = toSymbol(Terminal::T_QUANT_NT_OR_MT_LAZY);
			SymbolType tQPG = toSymbol(Terminal::T_QUANT_PLUS_GREEDY);
			SymbolType tQPLZ = toSymbol(Terminal::T_QUANT_PLUS_LAZY);
			SymbolType tQQG = toSymbol(Terminal::T_QUANT_QMARK_GREEDY);
			SymbolType tQQLZ = toSymbol(Terminal::T_QUANT_QMARK_LAZY);
			SymbolType tQSG = toSymbol(Terminal::T_QUANT_STAR_GREEDY);
			SymbolType tQSLZ = toSymbol(Terminal::T_QUANT_STAR_LAZY);
			SymbolType tAlphaNum = toSymbol(Terminal::T_ALPHANUM);

			// <regex> -> <regex> | <anchor>
			prod = ProductionType{
				{vRegEx},
				{vRegEx, tOr, vAnch},
				index
			};
			grammar.pushProduction(prod);

			// <regex> -> <anchor>
			prod = ProductionType{
				{vRegEx},
				{vAnch},
				index++
			};
			grammar.pushProduction(prod);

			// <anchor> -> ^<anchor>
			prod = ProductionType{
				{vAnch},
				{tAnchCharet, vAnch},
				index++
			};
			grammar.pushProduction(prod);

			// <anchor> -> <anchor>$
			prod = ProductionType{
				{vAnch},
				{vAnch, tAnchDollar},
				index++
			};
			grammar.pushProduction(prod);

			// <anchor> -> <chonc>
			prod = ProductionType{
				{vAnch},
				{vConc},
				index++
			};
			grammar.pushProduction(prod);

			// <conc> -> <conc><quant>
			prod = ProductionType{
				{vConc},
				{vConc, vQuant},
				index++
			};
			grammar.pushProduction(prod);

			// <conc> -> <quant>
			prod = ProductionType{
				{vConc},
				{vQuant},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>*
			prod = ProductionType{
				{vQuant},
				{vQuant, tQSG},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>*?
			prod = ProductionType{
				{vQuant},
				{vQuant, tQSLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>+
			prod = ProductionType{
				{vQuant},
				{vQuant, tQPG},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>+?
			prod = ProductionType{
				{vQuant},
				{vQuant, tQPLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>?
			prod = ProductionType{
				{vQuant},
				{vQuant, tQQG},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>??
			prod = ProductionType{
				{vQuant},
				{vQuant, tQQLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int m, int n}
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tAlphaNum, tAlphaNum, tRBrace},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int m, int n}?
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tAlphaNum, tAlphaNum, tRBraceLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int n,}
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tAlphaNum, tRBrace},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int n,}?
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tAlphaNum, tRBraceLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int n}
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tRBrace},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <quant>{int n}?
			prod = ProductionType{
				{vQuant},
				{vQuant, tLBrace, tAlphaNum, tRBraceLZ},
				index++
			};
			grammar.pushProduction(prod);

			// <quant> -> <group>
			prod = ProductionType{
				{vQuant},
				{vGroup},
				index++
			};
			grammar.pushProduction(prod);

			// <group> -> (<regex>)
			prod = ProductionType{
				{vGroup},
				{tRParen, vRegEx, tLParen},
				index++
			};
			grammar.pushProduction(prod);

			// <group> -> <char_class>
			prod = ProductionType{
				{vGroup},
				{vCharClass},
				index++
			};
			grammar.pushProduction(prod);

			// <char_class> -> [<char_class_seq>]
			prod = ProductionType{
				{vCharClass},
				{tRBracket, vCharClassSeq, tLBracket},
				index++
			};
			grammar.pushProduction(prod);

			// <char_class> -> [^<char_class_seq>]
			prod = ProductionType{
				{vCharClass},
				{tRBracketCharet, vCharClassSeq, tLBracket},
				index++
			};
			grammar.pushProduction(prod);

			// <char_class> -> <esq_seq>
			prod = ProductionType{
				{vCharClass},
				{vEscapeSeq},
				index++
			};
			grammar.pushProduction(prod);

			//// <char_class_seq> -> <range><char_class_seq>
			//prod = ProductionType{
			//	{vCharClassSeq},
			//	{vRange, vCharClassSeq},
			//	index++
			//};
			//grammar.pushProduction(prod);

			// <char_class_seq> -> <esq_seq><char_class_seq>
			prod = ProductionType{
				{vCharClassSeq},
				{vEscapeSeq, vCharClassSeq},
				index++
			};
			grammar.pushProduction(prod);

			// <char_class_seq> -> <esq_seq>
			prod = ProductionType{
				{vCharClassSeq},
				{vEscapeSeq},
				index++
			};
			grammar.pushProduction(prod);

			// <range> -> <factor>-<factor>
			//prod = ProductionType{
			//	{vRange},
			//	{vFact},
			//	index++
			//};
			//grammar.pushProduction(prod);

			// <esq_seq> -> \<factor>
			prod = ProductionType{
				{vEscapeSeq},
				{tBackSlash, vFact},
				index++
			};
			grammar.pushProduction(prod);

			// <esq_seq> -> \<factor>
			prod = ProductionType{
				{vEscapeSeq},
				{vFact},
				index++
			};
			grammar.pushProduction(prod);

			// <esq_seq> -> \<factor>
			prod = ProductionType{
				{vFact},
				{tAlphaNum},
				index++
			};
			grammar.pushProduction(prod);

			return grammar;
		}

	}
}