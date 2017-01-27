#include "stdafx.h"
#include "HeaderWriter.h"
#include "ConstWriter.h"
#include "VariableWriter.h"
#include "EnumWriter.h"
#include "StructWriter.h"

HeaderWriter::HeaderWriter()
{
}

HeaderWriter::~HeaderWriter()
{
}

vector<Variable> filter(vector<Variable> vars, char flags) {
	vector<Variable> filtered;
	for (unsigned int i = 0; i < vars.size(); i++) {
		Variable v = vars.at(i);
		if (v.flags & flags) {
			filtered.push_back(v);
		}
	}
	return filtered;
}

void WriteVars(IndentationWriter &iw, vector<Variable> vars, char flags, string modifier) {
	vector<Variable> filtered = filter(vars, flags);
	if (filtered.size() == 0)
		return;
	VariableWriter vw;
	iw << modifier << ":" << endl;
	iw.IncIndent();
	for (unsigned int i = 0; i < filtered.size(); i++) {
		vw.WriteCode(iw, filtered.at(i));
	}
	iw << endl;
	iw.DecIndent();

}

void HeaderWriter::WriteCode(IndentationWriter &iw, Class c)
{
	//iw << "#pragma once" << endl << endl;
	if (c.constants.size() > 0) {
		ConstWriter cw;
		for (unsigned int i = 0; i < c.constants.size(); i++) {
			cw.WriteCode(iw, c.constants.at(i));
		}
		iw << endl;
	}

	if (c.enums.size() > 0) {
		EnumWriter ew;
		for (unsigned int i = 0; i < c.enums.size(); i++) {
			ew.WriteCode(iw, c.enums.at(i));
		}
		iw << endl;
	}

	if (c.structs.size() > 0) {
		StructWriter sw;
		for (unsigned int i = 0; i < c.structs.size(); i++) {
			sw.WriteCode(iw, c.structs.at(i));
			iw << endl;
		}
		iw << endl;
	}
	

	iw << "class " << c.name;
	if (!c.parentName.empty()) {
		iw << " : public " << c.parentName;
	}
	iw << endl << "{" << endl;
	WriteVars(iw, c.variables, mod_private, "private");
	WriteVars(iw, c.variables, mod_protected, "protected");
	WriteVars(iw, c.variables, mod_public, "public");
	iw << "}" << endl << endl;
}
