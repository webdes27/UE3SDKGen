#pragma once
#include "CodeWriter.h"
#include "SingleItemWriter.h"
class StructWriter : SingleItemWriter<Struct>
{
public:
	StructWriter();
	~StructWriter();
	void WriteCode(IndentationWriter& iw, Struct s);
};

