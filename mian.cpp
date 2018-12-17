#include <iostream>
#include "files.h"
#include "Object.h"
#include "SSL.h"
#include "Draw.h"
#include <ctime>

using namespace std;
string path = "../models";
vector<string> fileNames;

string getfilename() {
	fileNames = getListFiles(path, "*.obj");
	cout << "**************************Zhou.Jinhua 21821084**************************" << endl;
	for (int i = 0; i < fileNames.size(); ++i)
	{
		cout << i + 1 << ":" << fileNames[i].c_str() << endl;
	}
	cout << "choose one:";
	int fileID;

	while (true) {
		cin >> fileID;
		if (fileID >= fileNames.size() || fileID <= 0)
			cout << "choose error,try again:" << endl;
		else
			return fileNames[fileID - 1];
	}
}

int main()
{
	string  fileName = path + "/" + getfilename();
	Model model(fileName);
	//窗口大小
	int width = 1024, height = 768;

	SectionScanline sectionScanline;
	sectionScanline.setSize(width, height);
	// 运行算法
	sectionScanline.run(model);

	Draw draw(&model, &sectionScanline);
	draw.display();

	return 0;
}