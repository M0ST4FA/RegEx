#include "gtest/gtest.h"
#include "regex.h"

class QuantifierTest : public ::testing::Test
{

protected:
	void SetUp() override {

	}

	void TearDown() override {

	}

	using regex = m0st4fa::regex::RegularExpression;
	using regexRes = m0st4fa::regex::RegularExpressionResult;

	regex qmark{ R"(mm?)" };
	regex star{ R"(mm*m)" };

};


TEST_F(QuantifierTest, qmark) {

	// std::string_view does not record \0
	EXPECT_TRUE(qmark.match("m"));

	EXPECT_TRUE(qmark.match("mm"));

	EXPECT_TRUE(qmark.match("mmm"));

	EXPECT_TRUE(qmark.match("mostaafa"));

};

TEST_F(QuantifierTest, star) {

	// std::string_view does not record \0
	EXPECT_FALSE(star.match("m"));

	EXPECT_TRUE(star.match("mm"));

	EXPECT_TRUE(star.match("mmm"));

	EXPECT_FALSE(star.match("mostaafa"));

};

