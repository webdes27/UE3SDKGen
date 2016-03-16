#pragma once
#include "CodeWriter.h"
class HeaderWriter
{
public:
	HeaderWriter();
	~HeaderWriter();
	void WriteCode(IndentationWriter& iw, Class c);
};