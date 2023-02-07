#include "gtest/gtest.h"
#include "regex.h"

class ErrorTest : public ::testing::Test {

protected:
	void SetUp() override {

	}

	void TearDown() override {

	}

	using regex = m0st4fa::regex::RegularExpression;
	using regexRes = m0st4fa::regex::RegularExpressionResult;

};

TEST_F(ErrorTest, flags) {
	using namespace m0st4fa::regex;

	EXPECT_THROW({
		regex gg("", "gg");
	}, m0st4fa::regex::InvalidRegexFlags);


	EXPECT_THROW({
		regex gg = "//gy"_r;
		}, m0st4fa::regex::InvalidRegexFlags);

	EXPECT_NO_THROW({
		regex r = "/x/g"_r;
		});
	
	regex r = "/x/i"_r;
	EXPECT_TRUE(r.match("X"));

};

TEST_F(ErrorTest, op) {
	using namespace m0st4fa::regex;

	regex r = "/x/g"_r;
	EXPECT_TRUE(r.match("x"));

}