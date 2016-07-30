#pragma once
#include "CodeWriter.h"
#include "SingleItemWriter.h"
class ConstWriter : public SingleItemWriter<Const>
{
public:
	ConstWriter();
	~ConstWriter();
	void WriteCode(IndentationWriter& iw, Const c);
};

