#pragma once
#include "CodeWriter.h"
#include "SingleItemWriter.h"
class HeaderWriter : public SingleItemWriter<Class>
{
public:
	HeaderWriter();
	~HeaderWriter();
	void WriteCode(IndentationWriter& iw, Class c);
};