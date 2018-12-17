#include "Object.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//loading obj
bool Model::load(const string& path) {  
	string type;    
	ifstream file(path);   
	if (!file.is_open()) {
		cout << "Could not open the file " << endl;
		cout << "Program terminating.\n";
		exit(EXIT_FAILURE);
	}

	//read vertexes
	while (file >> type)   
	{
		if (type == "v") {
			Vertex vt;
			file >> vt.Point.x >> vt.Point.y >> vt.Point.z;
			Vertexes.push_back(vt);
		}
		//reda vn
		else if (type == "vn") {
			glm::vec3 vn;
			file >> vn.x >> vn.y >> vn.z;
			Normals.push_back(vn);
		}
		//read face
		else if (type == "f")
		{
			Face face;
			int vIndex, tIndex, vnIndex;
			int faceIndex = Faces.size();  

			while (true)
			{
				char ch = file.get();
				if (ch == ' ') continue;
				else if (ch == '\n' || ch == EOF) break;
				else file.putback(ch);

				file >> vIndex; 

				char splitter = file.get();
				vnIndex = 0;

				if (splitter == '/')
				{
					splitter = file.get();
					if (splitter == '/') {
						file >> vnIndex; 
					}
					else
					{
						file.putback(splitter);
						file >> tIndex; 
						splitter = file.get();
						if (splitter == '/') {
							file >> vnIndex;
						}
						else file.putback(splitter);
					}
				}
				else file.putback(splitter);

				face.vertexIndex.push_back(vIndex - 1);   
				face.normalIndex.push_back(vnIndex - 1); 
			}
			//normalnize
			if (face.vertexIndex.size() > 2) {   
				glm::vec3 a = Vertexes[face.vertexIndex[0]].Point,
					b = Vertexes[face.vertexIndex[1]].Point,
					c = Vertexes[face.vertexIndex[2]].Point;
				glm::vec3 u = glm::cross(b - a, c - b);
				glm::vec3 Normal = u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));

				face.normal = Normal;
				Faces.push_back(face);
			}
		}
	}
	file.close();
	return true;
}

Model::Model(const string& path)
{
	if (load(path))
	{
		string face_type;
		cout << "模型" + path + "加载成功！" << endl;
		cout << "Faces：" << Faces.size() <<"，Vertexes：" << Vertexes.size() << endl;
	}
	else
	{
		cout << "无法打开obj文件：" + path << endl;
	}
}

// rotation
void Model::rotate(float** RotateMat, vector<Vertex>& vertexes)
{
	int vertex_num = vertexes.size();
	for (int i = 0; i < vertex_num; ++i)
	{
		glm::vec3 point;
		glm::vec3 tmp_point = vertexes[i].Point - Center_Point;
		vertexes[i].Point.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		vertexes[i].Point.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		vertexes[i].Point.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
		vertexes[i].Point += Center_Point;
	}
	int normal_num = Normals.size();
	for (int i = 0; i < normal_num; ++i)
	{
		glm::vec3 tmp_point = Normals[i];
		Normals[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		Normals[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		Normals[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;

	}
	int face_num = Faces.size();
	for (int i = 0; i < face_num; ++i)
	{
		glm::vec3 tmp_point = Faces[i].normal;
		Faces[i].normal.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		Faces[i].normal.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		Faces[i].normal.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	}
}

void Model::modelResize(int width, int height) {
	glm::vec3 min_xyz(INFINITY, INFINITY, INFINITY),
		max_xyz(-INFINITY, -INFINITY, -INFINITY);
	glm::vec3 center_xyz(0.0f, 0.0f, 0.0f);
	int vertex_num = this->Vertexes.size();
	
	for (int i = 0; i < vertex_num; ++i)
	{
		const glm::vec3& vertex = this->Vertexes[i].Point;
		min_xyz.x = min(min_xyz.x, vertex.x);
		min_xyz.y = min(min_xyz.y, vertex.y);
		min_xyz.z = min(min_xyz.z, vertex.z);
		max_xyz.x = max(max_xyz.x, vertex.x);
		max_xyz.y = max(max_xyz.y, vertex.y);
		max_xyz.z = max(max_xyz.z, vertex.z);
		
	}
	
	center_xyz.x = (min_xyz.x + max_xyz.x) / 2;          
	center_xyz.y = (min_xyz.y + max_xyz.y) / 2;
	center_xyz.z = (min_xyz.z + max_xyz.z) / 2;

	float model_width = max_xyz.x - min_xyz.x;
	float model_height = max_xyz.y - min_xyz.y;
	float max_model_len = max(model_width, model_height);
	//zoom or small
	float scale = min(width, height) / max_model_len;
	scale = 0.6*scale;

	for (int i = 0; i < vertex_num; ++i)
	{
		glm::vec3& vertex_point = this->Vertexes[i].Point;
		
		vertex_point.x = (vertex_point.x - center_xyz.x)*scale + width / 2;
		vertex_point.y = (vertex_point.y - center_xyz.y)*scale + height / 2;
		vertex_point.z = (vertex_point.z - center_xyz.z)*scale;

	}
	this->Center_Point = glm::vec3(width / 2, height / 2, 0);
}