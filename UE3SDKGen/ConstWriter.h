#pragma once
#include "CodeWriter.h"
class ConstWriter
{
public:
	ConstWriter();
	~ConstWriter();
	void WriteCode(IndentationWriter& iw, Const c);
};

