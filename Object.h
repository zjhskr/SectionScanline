#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;
//vertex information
class Vertex {
public:
	glm::vec3 Point;  
	glm::vec3 Color;  
	glm::vec3 Normal; 
};
//face  information
class Face {
public:
	vector<int> vertexIndex;
	vector<int> normalIndex;    
	glm::vec3 normal;
	glm::vec3 Color;
};

//read the type of obj
class Model {
public:
	vector<Vertex> Vertexes;   
	vector<Face> Faces;        
	vector<glm::vec3> Normals;  
	glm::vec3 Center_Point;

	bool load(const std::string& path);
	Model() {};
	Model(const std::string& path);
	void rotate(float** RotateMat, vector<Vertex>& Vertexes);
	void modelResize(int width, int height);
};
