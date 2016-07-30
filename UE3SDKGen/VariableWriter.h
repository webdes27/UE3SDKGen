#pragma once
#include "CodeWriter.h"
#include "SingleItemWriter.h"

class VariableWriter : public SingleItemWriter<Variable>
{
public:
	VariableWriter();
	~VariableWriter();
	void WriteCode(IndentationWriter & iw, Variable v);
};

