//#include "gtest/gtest.h"
//#include "regex.h"
//
//TEST(TestCase1, Test1) {
//  EXPECT_EQ(1, 1);
//  EXPECT_TRUE(true);
//}
//
//TEST(TestCase1, Test2) {
//	using regex = m0st4fa::regex::RegularExpression;
//	using regexRes = m0st4fa::regex::RegularExpressionResult;
//
//	regex re{ R"(h*i+there\x*)", "g" }; // = RegularExpression {pattern, options}; // the regular expression is compiled by the constructor.
//	regexRes reres = re.exec("mostafa"); // you execute the compiled regular expression against the string
//	re.getPattern();
//	re.getFlags();
//	re.getIndex();
//
//	EXPECT_EQ(1, re.match("mostafa"));
//}