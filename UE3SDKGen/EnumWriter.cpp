#include "stdafx.h"
#include "EnumWriter.h"


EnumWriter::EnumWriter()
{
}


EnumWriter::~EnumWriter()
{
}

void EnumWriter::WriteCode(IndentationWriter & iw, Enum e)
{
	iw << "enum " << e.name << endl;
	iw << "{" << endl;
	iw.IncIndent();
	for (unsigned int i = 0; i < e.properties.size(); i++) {
		EnumProperty ep = e.properties.at(i);
		iw << ep.key << " = " << ep.value;
		if (i != e.properties.size() - 1) {
			iw << ",";
		}
		iw << endl;
	}
	iw.DecIndent();
	iw << "};" << endl;
}
