#include "gtest/gtest.h"
#include "regex.h"

class QMarkLazyTest : public ::testing::Test
{

protected:
	void SetUp() override {

	}

	void TearDown() override {

	}
	
	using regex = m0st4fa::regex::RegularExpression;
	using regexRes = m0st4fa::regex::RegularExpressionResult;

	regex re{ R"(mm?)", "g" }; 

};


TEST_F(QMarkLazyTest, match) {
	bool val = true;
		 
	// std::string_view does not record \0
	val = re.match("m");
	EXPECT_TRUE(val);

	val = re.match("mm");
	EXPECT_TRUE(val);

	val = re.match("mmm");
	EXPECT_FALSE(val);

	val = re.match("mostaafa");
	EXPECT_FALSE(val);
};
