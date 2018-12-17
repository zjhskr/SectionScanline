#include "Draw.h"
using namespace std;

SectionScanline* Draw::engine = NULL;
Model* Draw::model = NULL;
glm::vec3 Draw::point_1st = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 Draw::point_2rd = glm::vec3(0.0, 0.0, 0.0);
float** Draw::RotateMat = NULL;

int Draw::idx1 = -1, Draw::idx2 = -1;


Draw::Draw(Model* model, SectionScanline* slzBuffer)
{
	this->model = model;
	engine = slzBuffer;

	Drawer(*model);

	RotateMat = new float*[3];
	for (int i = 0; i < 3; ++i)
	{
		RotateMat[i] = new float[3];
		for (int j = 0; j < 3; ++j)
		{
			RotateMat[i][j] = 0.0f;
		}
	}
}

Draw::~Draw()
{
	for (int i = 0; i < 3; ++i)
	{
		delete[] RotateMat[i];
	}
	delete[] RotateMat;
	RotateMat = NULL;
}

void Draw::Drawer(Model& model) {               
	glm::vec3 light_color(0.5, 0.5, 0.5);
	glm::vec3 ambient_color(0.5, 0.5, 0.5);
	glm::vec3 light_position(300.0f, 400.0f, 500.0f);
	int face_num = model.Faces.size();

	for (int i = 0; i < face_num; ++i)
	{
		Face& face = model.Faces[i];
		int face_vertex_num = face.vertexIndex.size();
		
		for (int j = 0; j < face_vertex_num; ++j)
		{
			Vertex face_vertex = model.Vertexes[face.vertexIndex[j]];
																	
			glm::vec3 ray_direction, temp = (light_position - face_vertex.Point);
			ray_direction = temp * (1.0f / sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z));

			glm::vec3 normal = face.normalIndex[j] >= 0 ?
				model.Normals[face.normalIndex[j]] : face.normal;
			float cosine = dot(ray_direction, normal);
			if (cosine > 0.0)face.Color += kd * cosine*light_color;
			face.Color += ambient_color;
		}
		face.Color /= face.vertexIndex.size();

		if (face.Color.r > 1.0f)face.Color.r = 1.0f;
		if (face.Color.r < 0.0f)face.Color.r = 0.0f;
		if (face.Color.g > 1.0f)face.Color.g = 1.0f;
		if (face.Color.g < 0.0f)face.Color.g = 0.0f;
		if (face.Color.b > 1.0f)face.Color.b = 1.0f;
		if (face.Color.b < 0.0f)face.Color.b = 0.0f;
	}

}

void Draw::loop()
{
	int width = 0, height = 0;
	engine->getSize(width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	engine->run(*model);

	glBegin(GL_POINTS);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{

			glm::vec3 rgb(0.0f, 0.0f, 0.0f);
			int tmp = engine->idBuffer[y][x];
			if (tmp >= 0)
			{
				rgb = model->Faces[tmp].Color;
			}
			glColor3f(rgb.r, rgb.g, rgb.b);
			glVertex2i(x, y);
		}
	}
	glEnd();

	glFinish();
}

void Draw::reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	engine->setSize(w, h);
	(*model).modelResize(w, h);
	engine->run(*model);
}

void Draw::display()
{
	int width = 0, height = 0;
	engine->getSize(width, height);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 80);
	glutCreateWindow("Çø¼äÉ¨ÃèÏß");
	glutDisplayFunc(loop);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);

	glutMainLoop();
}

void Draw::keyboard(int key, int x, int y)
{
	float theta = 0.0;
	const float PI = 3.1415926;
	float dtheta = PI / 12;
	float n1 = 1.0, n2 = 0.0, n3 = 0.0;
	switch (key)
	{
	case GLUT_KEY_LEFT:
		theta = dtheta;
		n1 = 0.0;
		n2 = 1.0;
		n3 = 0.0;
		break;
	case GLUT_KEY_RIGHT:
		theta = -dtheta;
		n1 = 0.0;
		n2 = 1.0;
		n3 = 0.0;
		break;
	case GLUT_KEY_UP:
		theta = dtheta;
		n1 = 1.0;
		n2 = 0.0;
		n3 = 0.0;
		break;
	case GLUT_KEY_DOWN:
		theta = -dtheta;
		n1 = 1.0;
		n2 = 0.0;
		n3 = 0.0;
		break;
	case GLUT_KEY_PAGE_UP:
		theta = -dtheta;
		n1 = 0.0;
		n2 = 0.0;
		n3 = 1.0;
		break;
	case GLUT_KEY_PAGE_DOWN:
		theta = dtheta;
		n1 = 0.0;
		n2 = 0.0;
		n3 = 1.0;
		break;

	default:
		break;
	}

	float cosine = cos(theta), sine = sin(theta);

	RotateMat[0][0] = n1 * n1 + (1 - n1 * n1)*cosine;
	RotateMat[0][1] = n1 * n2*(1 - cosine) + n3 * sine;
	RotateMat[0][2] = n1 * n3*(1 - cosine) - n2 * sine;

	RotateMat[1][0] = n1 * n2*(1 - cosine) - n3 * sine;
	RotateMat[1][1] = n2 * n2*(1 - cosine) + cosine;
	RotateMat[1][2] = n2 * n3*(1 - cosine) + n1 * sine;

	RotateMat[2][0] = n1 * n2*(1 - cosine) + n2 * sine;
	RotateMat[2][1] = n2 * n3*(1 - cosine) - n1 * sine;
	RotateMat[2][2] = n3 * n3 + (1 - n3 * n3)*cosine;

	model->rotate(RotateMat, model->Vertexes);

	engine->needUpdate = true;
	engine->run(*model);
	glutPostRedisplay();
}
