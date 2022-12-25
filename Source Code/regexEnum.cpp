#include <string>
#include <format>

#include "regexEnum.h"

std::string m0st4fa::toString(const Terminal terminal)
{
    static const char* names[(size_t)Terminal::T_COUNT]{
        "OP_OR",
        "CARET",
        "DOLLAR",
        R"(\A)",
        R"(\z)",
        R"(\Z)",
        R"(\G)",
        "STAR_GREEDY",
        "STAR_LAZY",
        "PLUS_GREEDY",
        "PLUS_LAZY",
        "QMARK_GREEDY",
        "QMARK_LAZY",
        "{n}",
        "{n}?",
        "{n, }",
        "{n, }?",
        "{n,m}",
        "{n,m}?",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_BRACKET",
        "LEFT_BRACKET_CHARET",
        "RIGHT_BRACKET",
        "LEFT_BRACE",
        "RIGHT_BRACE",
        "RIGHT_BRACE_LAZY",
        "BACK_SLASH",
        "ALPHA",
        "NUM",
        "EPSILON",
        "EOF"
    };

    if (terminal == Terminal::T_COUNT)
        return std::format("Number of terminals: {}", (size_t)terminal);

    return names[(size_t)terminal];
}
std::string m0st4fa::stringfy(const Terminal terminal)
{
    return toString(terminal);
}

std::string m0st4fa::toString(const Variable variable)
{
    static const char* names[(size_t)Variable::NT_COUNT]{
        "REGEX",
        "REGEX'",
        "ANCHOR",
        "CONC",
        "QUANT",
        "GROUP",
        "CHAR_CLASS",
        "CHAR_CLASS_SEQ",
        "RANGE",
        "ESQ_SEQ",
        "FACTOR"
    };

    if (variable == Variable::NT_COUNT)
        return std::format("Number of variables: {}", (size_t)variable);

    return names[(size_t)variable];
}
std::string m0st4fa::stringfy(const Variable variable)
{
    return toString(variable);
}