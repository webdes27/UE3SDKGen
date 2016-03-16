#pragma once

#define LOL	123

enum 
{
	Yes = 0x0,
	No = 0x1,
	Maybe = 0x2
};

struct TestStruct
{
	unsigned int* test;
	int testi2;
	unsigned class UDistributionFloat* Distribution[0x80];
	string test2;
	string test3;
};

class TestClass : public object
{
private:
	unsigned int test;
	int testi2;
	unsigned int testi3[0x100];
	
public:
	string test2;
	string test3;
	string test4;
	
}
