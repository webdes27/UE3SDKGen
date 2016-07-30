#pragma once
#include "IndentationWriter.h"

template<typename T>
class SingleItemWriter
{
public:
	void WriteCode(IndentationWriter &iw, T t);
};

