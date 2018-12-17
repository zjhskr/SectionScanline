#pragma once
#include <GLFW/glfw3.h>
#include <glut.h>
#include "object.h"
#include "SSL.h"
#include "files.h"
#include <iostream>


class SectionScanline;

class Draw
{
public:

	Draw(Model* model, SectionScanline* zBuffer);
	~Draw();

	void display();

	static Model* model;
	static SectionScanline* engine;
	static glm::vec3 point_1st, point_2rd;
	static float** RotateMat;
	static int idx1, idx2;

	const float kd = 0.8;
	void Drawer(Model& model);

	static void loop();
	static void reshape(int w, int h);
	static void keyboard(int key, int x, int y);
	static void drag_mouse(int button, int state, int x, int y);

};