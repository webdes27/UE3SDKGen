#pragma once
#include "CodeWriter.h"
class StructWriter
{
public:
	StructWriter();
	~StructWriter();
	void WriteCode(IndentationWriter& iw, Struct s);
};

