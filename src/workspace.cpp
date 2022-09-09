#include "workspace.h"
#include "TextEditor.h"

Workspace::Workspace(std::string fpath) {
	FilePath = fpath;
	FileName = fpath.substr(fpath.find_last_of("/\\") + 1);
	Editor = new TextEditor();
}

Workspace::~Workspace() {
}
