#pragma once
#include <string>
#include <sstream>
using namespace std;
class IndentationWriter
{
private:
	
	void indent(int depth);
	void indent();
	bool nextLine = true;

public:
	stringstream sstream;
	IndentationWriter();
	IndentationWriter(const IndentationWriter &obj);
	~IndentationWriter();
	int currentIndentation = 0;
	void IncIndent();
	void DecIndent();

	void Write(char* s);
	void Write(string s);
	void Write(stringstream s);

	void WriteLine();
	void WriteLine(char* s);
	void WriteLine(string s);

	//void WriteLine(ostream& s);

	IndentationWriter& operator<<(std::ostream&(*f)(std::ostream&)) {
		if (nextLine) {
			indent();
			nextLine = false;
		}
		this->sstream << f;

		if (f == endl) {
			nextLine = true;
		}
		return *this;
	}

	IndentationWriter& operator<<(string & f) {
		if (nextLine) {
			indent();
			nextLine = false;
		}
		this->sstream << f;
		return *this;
	}

	IndentationWriter& operator<<(const string & f) {
		if (nextLine) {
			indent();
			nextLine = false;
		}
		this->sstream << f;
		return *this;
	}

	IndentationWriter& operator<<(int & f) {
		if (nextLine) {
			indent();
			nextLine = false;
		}
		this->sstream << std::hex << std::uppercase << "0x" <<  f << std::nouppercase << std::dec;
		return *this;
	}
};

