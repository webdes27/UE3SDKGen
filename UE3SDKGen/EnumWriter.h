#pragma once
#include "CodeWriter.h"
class EnumWriter
{
public:
	EnumWriter();
	~EnumWriter();
	void WriteCode(IndentationWriter &iw, Enum e);
};

