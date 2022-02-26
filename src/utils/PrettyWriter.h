#pragma once

#include <sstream>
#include <string>

class PrettyWriter {
private:
	std::stringstream ss;
	bool currentLineEmpty = true;
	
	int indentLevel;
	std::string indent;
	std::string indentText;

	void writeStringToStream(const std::string& s) {
		if (s == "\n") {
			currentLineEmpty = true;
			ss << "\n";
			return;
		}

		if (currentLineEmpty) ss << indent;
		currentLineEmpty = false;
		ss << s;
	}

public:
	PrettyWriter(std::string indentText) : indentText(indentText) {
		indentLevel = -1;
		indent = "";
	}

	void rawWrite(std::string& str) {
		ss << str;
	}

	void increaseIndent() {
		if (++indentLevel > 0) indent += indentText;
	}

	void decreaseIndent() {
		--indentLevel;
		if (!indent.empty()) indent = indent.substr(0, (int)indent.size() - (int)indentText.size());
	}

	void write(const std::vector<std::string>& args) {
		for (const std::string& s : args) {
			writeStringToStream(s);
		}
	}

	void writeNewLine() {
		ss << "\n";
		currentLineEmpty = true;
	}

	std::string getString() { return ss.str(); }
};
