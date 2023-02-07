#include "gtest/gtest.h"
#include "regex.h"

class FlagTest : public ::testing::Test
{

protected:
	void SetUp() override {

	}

	void TearDown() override {

	}

	using regex = m0st4fa::regex::RegularExpression;
	using regexRes = m0st4fa::regex::RegularExpressionResult;

	regex g{ R"(mm)", "g"};
	regex y{ R"(mm)", "y"};

};

TEST_F(FlagTest, globalFlag) {

	std::string_view str = "mm mm mm";
	std::string_view str2 = "mmostafa";

	EXPECT_TRUE(g.match(str));
	EXPECT_TRUE(g.match(str));
	ASSERT_TRUE(g.getIndex() == 5);

	EXPECT_FALSE(g.match(str2));
	EXPECT_TRUE(g.match(str2));

}

TEST_F(FlagTest, stickyFlag) {

	std::string_view str = "mm mm mm";
	std::string_view str2 = "mmostafa";

	EXPECT_TRUE(y.match(str));
	EXPECT_FALSE(y.match(str));
	ASSERT_TRUE(y.getIndex() == 0);

	EXPECT_TRUE(y.match(str2));
	y.setIndex(3);
	EXPECT_TRUE(y.match(str));

}
