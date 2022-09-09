#pragma once

#include <string>
#include "TextEditor.h"

class Workspace {
public:
	std::string FilePath;
	std::string FileName;
	TextEditor* Editor = nullptr;
	Workspace(std::string fpath);
	~Workspace();
};
