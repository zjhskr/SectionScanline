#pragma once
#include "Object.h"
#include "files.h"
#include <vector>
#include <algorithm>
#include <list>


using namespace std;

//edge structure
struct Edge
{
	float x;
	float dx;//-1/k
	int dy;
	int id;
};

//active  edge table
struct AET
{
	float x; 
	float dx; // -1/k
	int dy; 
	float z; 
	float dzx; // dzx = -a/c(c!=0)
	float dzy; // dzy = b/c(c!=0)
	int id; 
};

struct polygon
{
	float a, b, c, d;//a^2+b^2+c^2=1
	int id;
	int dy;
	bool inflag;
};

class SectionScanline
{
public:
	SectionScanline() {};
	~SectionScanline()
	{
		release();
	}
	void setSize(int width, int height);
	void getSize(int& width, int& height);
	void run(Model& model);

	int** idBuffer;   
	bool needUpdate;

private:
	int width, height;

	vector<polygon> polygonIDTable;   
	vector<list<Edge>> edgeTable;    
	vector<AET> ActiveEdgeTable;   
	list<int> inPolygonList;

	void release();
	void build(const Model& model);
	void addEdge(int y);

};
