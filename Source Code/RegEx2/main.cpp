#include "regex.h"



int main(void) {
	
	regex re; // = RegularExpression {pattern, options}; // the regular expression is compiled by the constructor.
	regexRes reres; // re.exec(string); // you execute the compiled regular expression against the string
	bool match; // re.match(string);    // asserts that the string matches the regular expression
	// re.getSource() => gets the source code of the regex that has been compiled
	// re.getFlags() => returns the flags applied to the regex
	// re.getIndex() => returns the index into the string of the next character that will be matched by the regex
	

	return 0;
}