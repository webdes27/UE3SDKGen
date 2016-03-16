#include "stdafx.h"
#include "ConstWriter.h"


ConstWriter::ConstWriter()
{
}


ConstWriter::~ConstWriter()
{
}

void ConstWriter::WriteCode(IndentationWriter & iw, Const c)
{
	iw << "#define " << c.key << "\t" << c.value << endl;
}
