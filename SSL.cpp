#include "SSL.h"
using namespace std;

void SectionScanline::release()  //内存释放
{
	if (idBuffer != NULL)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] idBuffer[i];
			idBuffer[i] = NULL;
		}
	}
	delete[] idBuffer;
	idBuffer = NULL;
}

void SectionScanline::setSize(int width, int height)
{
	if (width == this->width && height == this->height) return;

	release();

	this->width = width;
	this->height = height;
	needUpdate = true;

	idBuffer = new int*[height];        //二维数组动态内存分配
	for (int i = 0; i < height; ++i)
	{
		idBuffer[i] = new int[width];
	}
}

void SectionScanline::getSize(int& width, int& height)
{
	width = this->width;
	height = this->height;
}



//build active edge table
void SectionScanline::build(const Model& model)
{
	polygonIDTable.clear();
	edgeTable.clear();
	edgeTable.resize(height);

	int faces_size = model.Faces.size();

	for (int i = 0; i < faces_size; ++i)
	{
		float  min_y = INFINITY, max_y = -INFINITY;

		const Face& face = model.Faces[i];
		polygon polygon;
		polygon.id = i;

		//构建分类边表
		const  vector<int>& vertexIndex = model.Faces[i].vertexIndex;
		for (int j = 0, vsize = vertexIndex.size(); j < vsize; ++j)
		{   //通过比较得到较大的上端点值
			glm::vec3 p_1 = model.Vertexes[vertexIndex[j]].Point, p_2;   
			if (j == vsize - 1) {
				p_2 = model.Vertexes[vertexIndex[0]].Point;
			}
			else p_2 = model.Vertexes[vertexIndex[j + 1]].Point;
			if (p_1.y < p_2.y)
			{
				swap(p_1, p_2);
			}

			Edge edge;

			edge.dy = round(p_1.y) - round(p_2.y);             
			if (round(p_1.y) < 0) continue;
			if (edge.dy <= 0) continue;

			edge.x = p_1.x;
			edge.dx = -(p_1.x - p_2.x) / (p_1.y - p_2.y);
		
			edge.id = polygon.id;
			edgeTable[round(p_1.y)].push_back(edge);

			min_y = min(p_2.y, min_y);
			max_y = max(p_1.y, max_y);
		}

		glm::vec3 v = model.Vertexes[face.vertexIndex[0]].Point;
		polygon.a = face.normal.x;
		polygon.b = face.normal.y;
		polygon.c = face.normal.z;
		polygon.d = -(polygon.a * v.x + polygon.b * v.y + polygon.c * v.z);
		polygon.dy = max_y - min_y; 
		polygon.inflag = false;
		polygonIDTable.push_back(polygon);
	}
}

static bool APTSortCmp(const AET& lEdge, const AET& rEdge)
{
	if (lEdge.x < rEdge.x) return true;
	else if (lEdge.x == rEdge.x)
	{
		if (lEdge.z < rEdge.z) return true;
	}
	return false;
}

void SectionScanline::addEdge(int y)
{
	if (edgeTable[y].size() == 0)
		return;
	for (list<Edge>::iterator it = edgeTable[y].begin(), end = edgeTable[y].end();
		it != end; it++)
	{
		polygon *a_polygon = &polygonIDTable[it->id];
		AET a_edge;
		a_edge.x = it->x;
		a_edge.dx = it->dx;
		a_edge.dy = it->dy;
		a_edge.id = it->id;

		if (a_polygon->c == 0) {    
			a_edge.z = 0;
			a_edge.dzx = 0;
			a_edge.dzy = 0;
		}
		else
		{
			a_edge.z = -(a_polygon->d + a_polygon->b * y + a_polygon->a * it->x) / a_polygon->c;
			a_edge.dzx = -(a_polygon->a / a_polygon->c);
			a_edge.dzy = a_polygon->b / a_polygon->c;
		}
		ActiveEdgeTable.push_back(a_edge);
	}
}

void SectionScanline::run(Model& model)
{
	
	if (!needUpdate) return;

	model.modelResize(width, height);

	build(model);
	
	ActiveEdgeTable.clear();

	for (int y = height - 1; y >= 0; --y)
	{
		inPolygonList.clear();
		memset(idBuffer[y], -1, sizeof(int)*width);

		addEdge(y);

		if (ActiveEdgeTable.size() % 2 == 1)  
		{
			cout << "error num in the active edge table" << endl;
		}

		if (ActiveEdgeTable.size() == 0) continue;

		sort(ActiveEdgeTable.begin(), ActiveEdgeTable.end(), APTSortCmp);

		for (int ae = 0; ae < ActiveEdgeTable.size(); ++ae)
		{
			AET& edge1 = ActiveEdgeTable[ae];
			if (ae == ActiveEdgeTable.size() - 1)
			{
				polygonIDTable[edge1.id].inflag = false;
				inPolygonList.remove(edge1.id);
				break;
			}

			AET& edge2 = ActiveEdgeTable[ae + 1];
			if (!polygonIDTable[edge1.id].inflag)
			{
				polygonIDTable[edge1.id].inflag = true;
				inPolygonList.push_back(edge1.id);
			}
			else
			{
				polygonIDTable[edge1.id].inflag = false;
				inPolygonList.remove(edge1.id);
			}
			if (edge1.x == edge2.x) {
				continue;
			}
			int mid_x = (edge1.x + edge2.x) / 2;   

			int polygon_id = -1;
			float Z = -INFINITY;
			float temZ;

			if (inPolygonList.size() == 0)
				continue;

			if (inPolygonList.size() == 1) {
				polygon_id = inPolygonList.front();
			}
			else if (inPolygonList.size() >= 2)
			{
				list<int>::iterator it_begin = inPolygonList.begin();
				list<int>::iterator it_end = inPolygonList.end();

				for (; it_begin != it_end; ++it_begin)
				{
					polygon& active_polygon = polygonIDTable[*it_begin];
					if (active_polygon.c == 0) {
						temZ = 0;
					}
					else {
						temZ = -(active_polygon.d + active_polygon.a * (mid_x)+active_polygon.b * y) / active_polygon.c;

					}
					if (Z < temZ)   
					{
						Z = temZ;
						polygon_id = active_polygon.id; 
					}
				}
			}
			if (polygon_id >= 0) {
				for (int x = edge1.x, j = edge2.x; x < j; ++x)
				{
					idBuffer[y][x] = polygon_id;
				}
			}
		}
		vector<AET> tempAET;
		for (int ae = 0; ae < ActiveEdgeTable.size(); ++ae) {
			AET& edge = ActiveEdgeTable[ae];
			--edge.dy;
			if (edge.dy <= 0) {
				continue;
			}
			edge.x += edge.dx;
			edge.z += edge.dzx * edge.dx + edge.dzy; //z =  z + dzx*dx + dzy
			tempAET.push_back(edge);
		}
		ActiveEdgeTable.clear();
		ActiveEdgeTable.assign(tempAET.begin(), tempAET.end());
		
		inPolygonList.clear();
	}
	needUpdate = false;
}
