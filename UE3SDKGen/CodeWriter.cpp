#include "stdafx.h"
#include "IndentationWriter.h"
#include "CodeWriter.h"
#include "StructWriter.h"
#include <sstream>
#include <fstream>
#include "HeaderWriter.h"
CodeWriter::CodeWriter(string path)
{
	this->path = path;
}

CodeWriter::~CodeWriter()
{
}

void CodeWriter::AddClass(Class c)
{
	classes.push_back(c);
}

void CodeWriter::flushToFile(std::string fileName, IndentationWriter* iw)
{
	ofstream codeFile(path + fileName, ios_base::out);
	codeFile << iw->sstream.rdbuf();
	codeFile.close();
	iw->sstream.clear();
}

void CodeWriter::Save()
{
	HeaderWriter hw;
	StructWriter sw;

	IndentationWriter* iw = new IndentationWriter();
	iw->WriteLine("#pragma once");
	for (unsigned int i = 0; i < structs.size(); i++) {
		sw.WriteCode(*iw, structs.at(i));
	}
	flushToFile("_structs.h", iw);

	iw->WriteLine("#pragma once");
	for (unsigned int i = 0; i < classes.size(); i++) {
		hw.WriteCode(*iw, classes.at(i));
	}
	flushToFile("_classes.h", iw);
}