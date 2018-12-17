#include "files.h"
using namespace std;

 vector<string> getListFiles(string path, string suffix) {
	_finddata_t file;
	intptr_t handle;

	string file_type;
	if (path.substr(0, 1) == "." || path.substr(0, 2) == "..")file_type = path + "/" + suffix;
	else file_type = path + "\\" + suffix;
	const char* cfile_type = file_type.c_str();

	int k = handle = _findfirst(cfile_type, &file);
	int i = 0;
	if (k == -1)
	{
		cout << "²»´æÔÚ" + suffix << endl;
		system("pause");
		exit(-1);
	}
	vector<string> filenames;
	while (k != -1)
	{
		filenames.push_back(string(file.name));
		k = _findnext(handle, &file);
	}
	_findclose(handle);
	return filenames;
};
