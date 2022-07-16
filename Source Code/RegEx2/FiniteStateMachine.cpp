#include "FiniteStateMachine.h"

std::ostream& m0st4fa::operator<<(const std::ostream& os, const FSMResult& result)
{
    printf("Accepted string: %i\nIndecies of the match: {%u, %u}\n", 
        result.accepted, result.indecies.start, result.indecies.end);

#ifdef _DEBUG
    printf("Matched string: %.*s\n", result.indecies.end - result.indecies.start, result.input.c_str() + result.indecies.start - 1);
#endif

    return std::cout;
}
