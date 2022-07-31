
#include "FiniteStateMachine.h"


std::ostream& m0st4fa::operator<<(const std::ostream& os, const FSMResult& result)
{
	
    std::string temp = "{ ";
    temp += std::to_string(*result.finalState.begin());
    for (auto s : result.finalState) {
        if (s == *result.finalState.begin())
            continue;
		
        temp += (", " + std::to_string(s));
    }
    temp += " }";
	
    printf("Accepted string: %s\nIndecies of the match: { %u, %u }\nFinal states reached: %s\n", 
        result.accepted ? "true" : "false", result.indecies.start, result.indecies.end, temp.data());

#ifdef _DEBUG
    printf("Matched string: %.*s\n", result.indecies.end - result.indecies.start, result.input.c_str() + result.indecies.start);
#endif

    return std::cout;
}
