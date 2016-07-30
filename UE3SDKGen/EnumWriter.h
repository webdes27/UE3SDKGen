#pragma once
#include "CodeWriter.h"
#include "SingleItemWriter.h"
class EnumWriter : public SingleItemWriter<Enum>
{
public:
	EnumWriter();
	~EnumWriter();
	void WriteCode(IndentationWriter &iw, Enum e);
};

