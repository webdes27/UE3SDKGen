#pragma once
#include "CodeWriter.h"
class VariableWriter
{
public:
	VariableWriter();
	~VariableWriter();
	void WriteCode(IndentationWriter & iw, Variable v);
};

