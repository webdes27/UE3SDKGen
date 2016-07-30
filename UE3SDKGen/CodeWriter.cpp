#include "stdafx.h"
#include "IndentationWriter.h"
#include "CodeWriter.h"
#include "StructWriter.h"
#include <sstream>
#include <fstream>
#include "HeaderWriter.h"
CodeWriter::CodeWriter(string file)
{
	this->name = file;
}

CodeWriter::~CodeWriter()
{
}

void CodeWriter::AddClass(Class c)
{
	classes.push_back(c);
}


void CodeWriter::Save()
{
	HeaderWriter hw;
	StructWriter sw;

	ofstream myFile(name, ios_base::out);

	IndentationWriter* iw = new IndentationWriter();
	for (unsigned int i = 0; i < structs.size(); i++) {
		sw.WriteCode(*iw, structs.at(i));
	}

	for (unsigned int i = 0; i < classes.size(); i++) {
		hw.WriteCode(*iw, classes.at(i));
	}
	//string code = iw->GetCode();
	myFile << iw->sstream.rdbuf();

	myFile.close();
}