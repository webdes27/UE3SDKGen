#include "stdafx.h"
#include "IndentationWriter.h"



void IndentationWriter::indent(int depth)
{
	for (int i = 0; i < depth; i++)
		sstream << "\t";
}

void IndentationWriter::indent()
{
	indent(currentIndentation);
}

IndentationWriter::IndentationWriter()
{
}

IndentationWriter::IndentationWriter(const IndentationWriter & obj)
{
}


IndentationWriter::~IndentationWriter()
{

}

void IndentationWriter::IncIndent()
{
	currentIndentation++;
}

void IndentationWriter::DecIndent()
{
	if (currentIndentation > 0)
		currentIndentation--;
}

void IndentationWriter::Write(char * s)
{
	sstream << s;
}

void IndentationWriter::Write(string s)
{
	sstream << s;
}

void IndentationWriter::Write(stringstream s)
{
	sstream << s.str();
}

void IndentationWriter::WriteLine()
{
	nextLine = true;
	sstream << endl;
}

void IndentationWriter::WriteLine(char * s)
{
	Write(s);
}

void IndentationWriter::WriteLine(string s)
{
	Write(s);
}


//void IndentationWriter::WriteLine(ostream & s)
//{
//	indent();
//	sstream << s. << endl;
//}


