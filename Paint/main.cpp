/*
*  File: main.cpp
*  Authors: Kyle Loveless, Charlie Miller, and Cornelius Donley
___________                     ____   ____     .__               __
\__    ___/___ _____    _____   \   \ /   /____ |  |___  __ _____/  |______
  |    |_/ __ \\__  \  /     \   \   Y   // __ \|  |\  \/ // __ \   __\__  \
  |    |\  ___/ / __ \|  Y Y  \   \     /\  ___/|  |_\   /\  ___/|  |  / __ \_
  |____| \___  >____  /__|_|  /    \___/  \___  >____/\_/  \___  >__| (____  /
             \/     \/      \/                \/               \/          \/
*/

// a simple paint program, soon to become A-Dopy-Illustrator V1
// S. Renk 
// program is in serious need of an upgrade
// it needs:
//     persistence to be able to capture drawn objects so they stay on the screen
//     the ability to load and save images created
//     rubberbanding 

// include needed libraries
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "CShape.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

#define NULL 0

// defins shapes
#define LINE 1
#define RECTANGLE 2
#define POLYGON  3
#define POINTS 4
#define TEXT 5
#define CIRCLE 6
#define ARC 7
#define SAVE 8
#define LOAD 9
#define CIRCLE 10
#define LINESTRIP 11

// function prototypes
void mouse(int, int, int, int);
void key(unsigned char, int, int);
void display(void);
void drawSquare(int, int);
void myReshape(GLsizei, GLsizei);
void myinit(void);
void screen_box(int, int, int, int, float, float, float);
void right_menu(int);
void color_menu(int);
void fill_menu(int);
void menu_area();
void rubberband(int, int);
void motion(int, int);
int pick(int, int);
void displayText(int, int, std::string, bool);
std::ostream& operator<<(std::ostream& os, const CShape& shape);
std::istream& operator>>(std::istream& is, CShape& shape);

//global vars program state vars
CONST float WIDTH = 800, HEIGHT = 500;
GLsizei wh = (GLsizei)WIDTH, ww = (GLsizei)HEIGHT;  // window size
GLfloat size = 3.0;          // half side length of square
int draw_mode = NULL, posArc = 1;        // current drawing mode 
int rx, ry, menuBtnSize, mousePosX, mousePosY;       //raster position (rx, ry)
													 //global vars for rendering engine state
GLfloat r = 0.0, g = 0.0, b = 0.0; // current drawing color 
bool fill = true, drag = false;           // draw solid or outlines
CShape *shapes, *newShape, *tempShape, *rubberShape = NULL, *redSlider, *greenSlider, *blueSlider;
CCoord * vertexOne, *vertexTwo;
static int count;  // # of verticies in current figure
int points[100][2];
int point[2];
void * font = GLUT_BITMAP_9_BY_15;
std::string userInput = "", messageLog = "";
std::string modeArray[] = { "Circle", "Arc", "Polygon", "Text", "Line\n Strip", "Rect", "Point", "Line" };
//std::fstream myfile;

void motion(int x, int y)
{
	float temp = (float)x;
	y = wh - y;

	mousePosX = x;
	mousePosY = y;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (redSlider->isPicked(mousePosX, mousePosY))
	{
		drag = true;
		delete redSlider;
		redSlider = new CRect();
		if (mousePosX < 3)
		{
			r = 0.0f;
			vertexOne = new CCoord(3, 70, 0, 1.0f, 0, 0.0f, 1.0f);
			vertexTwo = new CCoord(10, 95, 0, 1.0f, 0, 0.0f, 1.0f);

		}
		else if (mousePosX < 134 || r == 1.0f)
		{
			r = temp / 134;
			vertexOne = new CCoord(mousePosX - 4, 70, 0, 1.0f, 0, 0.0f, 1.0f);
			vertexTwo = new CCoord(mousePosX + 4, 95, 0, 1.0f, 0, 0.0f, 1.0f);

		}
		else
		{
			r = 1.0;
			vertexOne = new CCoord(130, 70, 0, 1.0f, 0, 0.0f, 1.0f);
			vertexTwo = new CCoord(137, 95, 0, 1.0f, 0, 0.0f, 1.0f);
		}

		redSlider->insertVertex(vertexOne);
		redSlider->insertVertex(vertexTwo);
	}
	else if (greenSlider->isPicked(mousePosX, mousePosY))
	{
		drag = true;
		delete greenSlider;
		greenSlider = new CRect();
		if (mousePosX < 3)
		{
			g = 0.0f;
			vertexOne = new CCoord(3, 36, 0, 0, 1.0f, 0.0f, 1.0f);
			vertexTwo = new CCoord(10, 61, 0, 0, 1.0f, 0.0f, 1.0f);
		}
		else if (mousePosX < 134 || g == 1.0f)
		{
			g = (temp / 134);
			vertexOne = new CCoord(mousePosX - 4, 36, 0, 0, 1.0f, 0.0f, 1.0f);
			vertexTwo = new CCoord(mousePosX + 4, 61, 0, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			g = 1.0f;
			vertexOne = new CCoord(130, 36, 0, 0, 1.0f, 0.0f, 1.0f);
			vertexTwo = new CCoord(137, 61, 0, 1.0f, 0.0f, 1.0f);
		}

		greenSlider->insertVertex(vertexOne);
		greenSlider->insertVertex(vertexTwo);
	}
	else if (blueSlider->isPicked(mousePosX, mousePosY))
	{
		drag = true;
		delete blueSlider;
		blueSlider = new CRect();
		if (mousePosX < 3)
		{
			b = 0.0f;
			vertexOne = new CCoord(3, 3, 0, 0.0f, 0.0f, 1.0f, 0.0f);
			vertexTwo = new CCoord(10, 28, 0, 0.0f, 0.0f, 1.0f, 0.0f);
		}
		else if (mousePosX < 134 || b == 1.0f)
		{
			b = temp / 134;
			vertexOne = new CCoord(mousePosX - 4, 3, 0, 0.0f, 0.0f, 1.0f, 0.0f);
			vertexTwo = new CCoord(mousePosX + 4, 28, 0, 0.0f, 0.0f, 1.0f, 0.0f);

		}
		else
		{
			b = 1.0f;
			vertexOne = new CCoord(130, 3, 0, 0.0f, 0.0f, 1.0f, 0.0f);
			vertexTwo = new CCoord(137, 28, 0, 0.0f, 0.0f, 1.0f, 0.0f);
		}

		blueSlider->insertVertex(vertexOne);
		blueSlider->insertVertex(vertexTwo);
	}
	glPopAttrib();
	glutPostRedisplay();
}

void rubberband(int x, int y)
{
	y = wh - y;
	mousePosX = x;
	mousePosY = y;
	displayText(145, wh - 15, "", false);
	
	int x1 = points[0][0];
	int y1 = points[0][1];
	int x2 = x;
	int y2 = y;
	double len = 0;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (count)
	{
		switch (draw_mode)
		{
		case(LINE) :
			rubberShape = new CLine();
			rubberShape->size = (int)size;

			len = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

			messageLog = "Origin: " + std::to_string(x1) + ", " + std::to_string(y1) + " Length: " + std::to_string((int)len) + "px";
			break;
		case(LINESTRIP) : 
			rubberShape = new CLine();
			rubberShape->size = (int)size;

			len = sqrt(pow(x - points[count - 1][0], 2) + pow(y - points[count - 1][1], 2));
			messageLog = "Length of Current Segment: " + std::to_string((int)len) + "px \n Double-Click to place final vertex (max 10).";
			break;
		case(RECTANGLE) :
			rubberShape = new CRect();
			
			messageLog = "Height: " + std::to_string((int)abs(y2-y1)) + "px Width: " + std::to_string((int)abs(x2-x1)) + "px Area: " + 
				std::to_string((int)abs((y2-y1) * (x2-x1))) + "px sq.";
			break;
		case(POLYGON) :
			if (count > 1)
				rubberShape = new CPoly();
			else
				rubberShape = new CLine();
			
			messageLog = "Number of Vertices: " + std::to_string(count) + "\n Double-Click to place final vertex (max 10).";
			break;
		case(CIRCLE) : rubberShape = new CCircle();

			len = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

			messageLog = "Origin: " + std::to_string(x1) + ", " + std::to_string(y1) + " Radius: " + std::to_string((int)len) + "px";
			break;
		case(ARC) :
			if (count > 1)
			{
				rubberShape = new CArc();
				rubberShape->size = posArc;
			}
			else
				rubberShape = new CLine();
			len = sqrt(pow(points[1][0] - points[0][0], 2) + pow(points[1][1] - points[0][1], 2));

			double theta = atan2(points[1][1] - points[0][1], points[1][0] - points[0][0]);
			double theta2 = atan2(y - points[0][1], x - points[0][0]);
			if (theta < 0) theta += 2.0 * 3.14159;
			if (theta2 < 0) theta2 += 2.0 * 3.14159;
			if (theta > theta2) theta2 += 2.0 * 3.14159;

			theta = theta * 180 / 3.14159;
			theta2 = theta2 * 180 / 3.14159;

			if (posArc)
				messageLog = "Origin: " + std::to_string(x1) + ", " + std::to_string(y1) + " Radius: " + std::to_string(int(len)) + "px Angle: " +
					std::to_string(int(theta2-theta)) + " degrees \n Press SPACEBAR to switch to negative degrees.";
			else
				messageLog = "Origin: " + std::to_string(x1) + ", " + std::to_string(y1) + " Radius: " + std::to_string(int(len)) + "px Angle: " +
				std::to_string(abs(int(theta - theta2)) - 360) + " degrees \n Press SPACEBAR to switch to positive degrees.";
			break;
		}
		rubberShape->fill = fill;
		for (int i = 0; i < count; i++)
			rubberShape->insertVertex(new CCoord(points[i][0], points[i][1], 0, r, g, b));
		rubberShape->insertVertex(new CCoord(x, y, 0, r, g, b));
	}
	glEnd();
	glPopAttrib();
	glutPostRedisplay();
}

void drawSquare(int x, int y)
{
	y = wh - y;
	glColor3ub((char)rand() % 256, (char)rand() % 256, (char)rand() % 256);
	glBegin(GL_POLYGON);
	glVertex2f(x + size, y + size);
	glVertex2f(x - size, y + size);
	glVertex2f(x - size, y - size);
	glVertex2f(x + size, y - size);
	glEnd();
}

// rehaping routine called whenever window is resized or moved 
void myReshape(GLsizei w, GLsizei h)
{
	// adjust clipping box
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// adjust viewport and  clear 
	
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();

	// set global size for use by drawing routine
	ww = w; wh = h;
}

void myinit(void)
{
	glViewport(0, 0, ww, wh);
	// Pick 2D clipping window to match size of X window 
	// This choice avoids having to scale object coordinates
	// each time window is resized 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);  // set clear color to black and clear window 
	glClear(GL_COLOR_BUFFER_BIT);
}

void mouse(int btn, int state, int x, int y)  //defines the drawing shapes
{
	y = wh - y;
	mousePosX = x;
	mousePosY = y;
	
	int where;
	static int xp[2], yp[2]; // x/y location of verticies

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		where = pick(x, y);
		glColor3f(r, g, b);
		if (where != 0)
		{
			count = 0;
			draw_mode = where;
		}
		else switch (draw_mode)
		{
			case(LINE) :
				if (count == 0)
				{
					count++;
					points[0][0] = x;
					points[0][1] = y;
				}
				else
				{
					newShape = new CLine();
					newShape->fill = fill;
					newShape->size = (int)size;
					newShape->insertVertex(new CCoord(points[0][0], points[0][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				break;
			case(LINESTRIP) :
				if (count < 9 && xp[0] == x && yp[0] == y)
				{
					newShape = new CLine();
					newShape->size = (int)size;
					for (int i = 0; i < count; i++)
						newShape->insertVertex(new CCoord(points[i][0], points[i][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				else if (count < 9)
				{
					points[count][0] = xp[0] = x;
					points[count][1] = yp[0] = y;
					count++;
				}
				else
				{
					newShape = new CLine();
					newShape->size = (int)size;
					for (int i = 0; i < count; i++)
						newShape->insertVertex(new CCoord(points[i][0], points[i][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				break;
			case(RECTANGLE) :
				if (count == 0)
				{
					count++;
					points[0][0] = x;
					points[0][1] = y;
				}
				else
				{
					newShape = new CRect();
					newShape->fill = fill;
					newShape->insertVertex(new CCoord(points[0][0], points[0][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				break;
			case(POINTS) :
				newShape = new CPoint();
				newShape->fill = fill;
				newShape->size = (int)size;
				newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
				CShape::insertShape(shapes, newShape);
				break;
			case(TEXT) :
				if (userInput.length())
				{
					newShape = new CText();
					points[0][0] = x;
					points[0][1] = y;
					newShape->insertVertex(new CCoord(points[0][0], points[0][1], 0, r, g, b));
					newShape->text = userInput;
					CShape::insertShape(shapes, newShape);
					userInput = "";
					messageLog = "\n\n Please start typing.";
				}
				break;
			case(ARC) :
				switch (count)
				{
				case(0) :
					count++;
					points[0][0] = x;
					points[0][1] = y;
					break;
				case(1) :
					count++;
					points[1][0] = x;
					points[1][1] = y;
					break;
				case(2) :
					newShape = new CArc();
					newShape->fill = fill;
					newShape->size = posArc;
					newShape->insertVertex(new CCoord(points[0][0], points[0][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(points[1][0], points[1][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
					  break;
			case(POLYGON) :
				if (count < 9 && xp[0] == x && yp[0] == y)
				{
					newShape = new CPoly();
					newShape->fill = fill;
					for (int i = 0; i < count; i++)
						newShape->insertVertex(new CCoord(points[i][0], points[i][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				else if (count < 9)
				{
					points[count][0] = xp[0] = x;
					points[count][1] = yp[0] = y;
					count++;
				}
				else
				{
					newShape = new CPoly();
					newShape->fill = posArc;
					for (int i = 0; i < count; i++)
						newShape->insertVertex(new CCoord(points[i][0], points[i][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					count = 0;
				}
				break;
			case(CIRCLE) :
				if (count == 0)
				{
					count++;
					points[0][0] = x;
					points[0][1] = y;
				}
				else
				{
					newShape = new CCircle();
					newShape->fill = fill;
					newShape->insertVertex(new CCoord(points[0][0], points[0][1], 0, r, g, b));
					newShape->insertVertex(new CCoord(x, y, 0, r, g, b));
					CShape::insertShape(shapes, newShape);
					delete rubberShape;
					rubberShape = NULL;
					count = 0;
				}
				break;
		}
		glPopAttrib();
	}
}

int pick(int x, int y)  //which button is selected?
{
	//y = wh - y;
	if (x > 0 && x < 70 && y > wh - 20 && y < wh)
	{
		std::ofstream myfile("picture.pic");
		tempShape = shapes;
		while (tempShape)
		{
			myfile << (*tempShape);
			tempShape = tempShape->nextShape;
		}
		myfile.close();
		messageLog = "Your complex picture has been saved!";
		return SAVE;
	}
	else if (x > 70 && x < 140 && y > wh - 20 && y < wh)
	{
		std::string line, line2;
		std::string file = "picture.pic";
		std::ifstream myfile("picture.pic");
		std::vector<std::string> temp;
		int pos = 0;
		if (myfile)
		{
			while (std::getline(myfile, line))
			{
				while (line.find(',') != std::string::npos)
				{
					pos = line.find(',');
					temp.push_back(line.substr(0, pos));
					line.erase(0, pos + 1);
				}
				switch (std::stoi(temp[0]))
				{
				case(CPOINT) : newShape = new CPoint(); break;
				case(CLINE) : newShape = new CLine(); break;
				case(CRECT) : newShape = new CRect(); break;
				case(CPOLY) : newShape = new CPoly(); break;
				case(CTEXT) : newShape = new CText(); break;
				case(CCIRCLE) : newShape = new CCircle(); break;
				case(CARC) : newShape = new CArc(); break;
				}
				newShape->fill = std::stoi(temp[1]);
				newShape->size = std::stoi(temp[2]);
				newShape->text = temp[3];
				for (int i = 0; i < std::stoi(temp[4]); i++)
				{
					newShape->insertVertex(new CCoord(std::stoi(temp[5 + i * 7]), std::stoi(temp[6 + i * 7]), std::stoi(temp[7 + i * 7]), //x,y,z
						std::stof(temp[8 + i * 7]), std::stof(temp[9 + i * 7]), std::stof(temp[10 + i * 7]), std::stof(temp[11 + i * 7]))); //r,g,b,a
				}
				CShape::insertShape(shapes, newShape);
				/*for (int i = 0; i < temp.size(); i++)
				printf("%s,", temp.at(i));
				printf("\n");*/
				temp.clear();
			}
			myfile.close();
		}
		else
			messageLog = "No file to open";
		messageLog = "Your complex picture has been loaded!";
		return LOAD;
	}
	else if (x > 0 && x < 70 && y > wh - 20 - menuBtnSize && y < wh - 20)
	{
		messageLog = "";
		return POINTS;
	}
	else if (x > 70 && x < 140 && y > wh - 20 - menuBtnSize && y < wh - 20) return LINE;
	else if (x > 0 && x < 70 && y > wh - 20 - menuBtnSize * 2 && y < wh - 20 - menuBtnSize) return LINESTRIP;
	else if (x > 70 && x < 140 && y > wh - 20 - menuBtnSize * 2 && y < wh - 20 - menuBtnSize) return RECTANGLE;
	else if (x > 0 && x < 70 && y > wh - 20 - menuBtnSize * 3 && y < wh - 20 - menuBtnSize * 2) return POLYGON;
	else if (x > 70 && x < 140 && y > wh - 20 - menuBtnSize * 3 && y < wh - 20 - menuBtnSize * 2)
	{
		userInput = "";
		messageLog = "\n\n Please start typing.";
		return TEXT;
	}
	else if (x > 0 && x < 70 && y > wh - 20 - menuBtnSize * 4 && y < wh - 20 - menuBtnSize * 3) return CIRCLE;
	else if (x > 70 && x < 140 && y > wh - 20 - menuBtnSize * 4 && y < wh - 20 - menuBtnSize * 3) return ARC;
	else return 0;
}

// draw a box at (x,y)
void screen_box(int x, int y, int l, int h, float red = 0.2, float green = 0.2, float blue = 0.2)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBegin(GL_QUADS);
	glColor3f(red, green, blue);
	glVertex2i(x, y);
	glVertex2i(x + l, y);
	glVertex2i(x + l, y + h);
	glVertex2i(x, y + h);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2i(x, y);
	glVertex2i(x + l, y);
	glVertex2i(x + l, y + h);
	glVertex2i(x, y + h);
	glEnd();
	glPopAttrib();
}

// mouse menu code
void right_menu(int id)
{
	if (id == 1) exit(0);
	else
	{
		tempShape = shapes;
		while (tempShape)
		{
			shapes = shapes->nextShape;
			delete tempShape;
			tempShape = shapes;
		}
		glutPostRedisplay();
	}
}

void clear_menu(int id)
{
	if (id == 1) //Clear last shape
	{
		tempShape = shapes;
		CShape * prevShape;
		if (tempShape)
		{
			if (!tempShape->nextShape)
				shapes = NULL;
			else
			{
				while (tempShape->nextShape)
				{
					prevShape = tempShape;
					tempShape = tempShape->nextShape;
				}
				prevShape->nextShape = NULL;
			}
			delete tempShape;
		}
		glutPostRedisplay();
	}
	if (id == 2) //Clear screen
	{
		tempShape = shapes;
		while (tempShape)
		{
			shapes = shapes->nextShape;
			delete tempShape;
			tempShape = shapes;
		}
		glutPostRedisplay();
	}
}

// set the current drawing color
void color_menu(int id)
{
	if (id == 1) { glClearColor(1.0f, 0.0f, 0.0f, 1.0f); }
	else if (id == 2) { glClearColor(0.0f, 1.0f, 0.0f, 1.0f); }
	else if (id == 3) { glClearColor(0.0f, 0.0f, 1.0f, 1.0f); }
	else if (id == 4) { glClearColor(0.0f, 1.0f, 1.0f, 1.0f); }
	else if (id == 5) { glClearColor(1.0f, 0.0f, 1.0f, 1.0f); }
	else if (id == 6) { glClearColor(1.0f, 1.0f, 0.0f, 1.0f); }
	else if (id == 7) { glClearColor(1.0f, 1.0f, 1.0f, 1.0f); }
	else if (id == 8) { glClearColor(0.0f, 0.0f, 0.0f, 1.0f); }
	else if (id == 9) { glClearColor(r, g, b, 1.0f); }
	glClear(GL_COLOR_BUFFER_BIT);
}

void point_size_menu(int id)
{
	if (id == 1) size = 1;
	else if (id == 2) size = 2;
	else if (id == 3) size = 3;
	else if (id == 4) size = 4;
	else if (id == 5) size = 5;
	else if (id == 6) size = 6;
	else if (id == 7) size = 7;
	else if (id == 8) size = 8;
	else if (id == 9) size = 9;
	else if (id == 10) size = 10;
}

// set fill mode
void fill_menu(int id)
{
	fill = (id == 1) ? 1 : 0;
}

// KB input
void key(unsigned char k, int xx, int yy)
{
	if (k == ' ' && draw_mode == ARC && posArc)
		posArc = 0;
	else if (k == ' ' && draw_mode == ARC)
		posArc = 1;
	if (draw_mode != TEXT) return;
	if (k == 8)
		userInput.pop_back();
	else
		userInput += k;
	messageLog = userInput + "\n\n Click the screen to place text at that location.";
	displayText(0, 200, userInput, true);
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2i(rx, ry);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, k);
	//glutStrokeCharacter(GLUT_STROKE_ROMAN,i);
	rx += glutBitmapWidth(GLUT_BITMAP_9_BY_15, k);

	glutPostRedisplay();
}

void displayText(int x, int y, std::string ss, bool default)
{
	std::string s;
	std::string temp;
	if (default)
		s = ss;
	else
	{
		float f = 0;
		s = "Mode: ";
		switch (draw_mode)
		{
			case(POINTS) : s += "Point"; break;
			case(LINE) : s += "Line"; break;
			case(LINESTRIP) : s += "Line Strip"; break;
			case(RECTANGLE) : s += "Rectangle"; break;
			case(POLYGON) : s += "Polygon"; break;
			case(CIRCLE) : s += "Circle"; break;
			case(ARC) : s += "Arc"; break;
			case(TEXT) : s += "Text"; break;
			case(NULL) : s += "None"; break;
			case(SAVE) : s += "Save"; break;
			case(LOAD) : s += "Load"; break;
		}

		s = s + ", Pixel Size: " + std::to_string((int)size);
		s = s + ", R: " + std::to_string(r).substr(0, 4);
		s = s + ", G: " + std::to_string(g).substr(0, 4);
		s = s + ", B: " + std::to_string(b).substr(0, 4);
		s = s + " X: " + std::to_string(mousePosX);
		s = s + " Y: " + std::to_string(mousePosY);
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2i(x, y);
	for (std::string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		if (c == '\n')
		{
			c = *++++i;
			glRasterPos2i(x, y - 20);
		}
		glutBitmapCharacter(font, c);
	}
	glPopAttrib();
}

void menu_area()
{
	int shift = 0;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(0.0, 0.0, 1.0);
	screen_box(0, wh - 20, 70, 20);
	displayText(15, wh - 15, "SAVE", true);//save

	screen_box(70, wh - 20, 70, 20);
	displayText(85, wh - 15, "LOAD", true);//load

	screen_box(140, wh - 20, ww - 90, 20); //status bar

	displayText(145, wh - 15, "", false);

	shift = wh - 80 - 20;
	screen_box(0, 80, 140, shift);//menu
	int temp = shift;
	menuBtnSize = (temp - 20) / 4;

	for (int x = 0; x < 4; x++)
	{
		screen_box(0, wh - temp + menuBtnSize* x, 70, menuBtnSize, 0.20f, 0.20f, 0.20f); //0, 2, 4, 6
		displayText(5, wh - temp + (menuBtnSize * x) + 40, modeArray[x * 2], true);

		screen_box(70, wh - temp + menuBtnSize* x, 70, menuBtnSize, 0.2f, 0.2f, 0.2f); //1, 3 , 5, 7
		displayText(75, wh - temp + menuBtnSize* x + 40, modeArray[x * 2 + 1], true);
	}

	screen_box(0, 0, 140, wh - shift, 0.2f, 0.2f, 0.2f);//controls

	temp = (wh - shift) / 3;
	// Draw red color line
	glBegin(GL_QUADS);
	glColor3f(0.0f, g, b);
	glVertex2i(5, 2 * temp + temp / 3);
	glVertex2i(5, 2 * temp + temp / 3 + 10);
	glColor3f(1.0f, g, b);
	glVertex2i(130, 2 * temp + temp / 3 + 10);
	glVertex2i(130, 2 * temp + temp / 3);

	glEnd();

	//draw a green color line
	glBegin(GL_QUADS);
	glColor3f(r, 0.0f, b);
	glVertex2i(5, temp + temp / 3);
	glVertex2i(5, temp + temp / 3 + 10);
	glColor3f(r, 1.0f, b);
	glVertex2i(130, temp + temp / 3 + 10);
	glVertex2i(130, temp + temp / 3);
	glEnd();

	//draw a blue color line
	glBegin(GL_QUADS);
	glColor3f(r, g, 0.0f);
	glVertex2i(5, temp / 3);
	glVertex2i(5, temp / 3 + 10);
	glColor3f(r, g, 1.0f);
	glVertex2i(130, temp / 3 + 10);
	glVertex2i(130, temp / 3);
	glEnd();

	if (drag == false)
	{
		redSlider = new CRect();
		vertexOne = new CCoord(3, 2 * temp + 3, 0, 1.0f, 0.0f, 0.0f, 1.0f);
		vertexTwo = new CCoord(10, temp * 3 - 5, 0, 0.0f, 0.0f, 1.0f, 1.0f);
		redSlider->insertVertex(vertexOne);
		redSlider->insertVertex(vertexTwo);

		greenSlider = new CRect();
		vertexOne = new CCoord(3, temp + 3, 0, 0.0f, 1.0f, 0.0f, 1.0f);
		vertexTwo = new CCoord(10, temp * 2 - 5, 0, 0.0f, 1.0f, 0.0f, 1.0f);
		greenSlider->insertVertex(vertexOne);
		greenSlider->insertVertex(vertexTwo);

		blueSlider = new CRect();
		vertexOne = new CCoord(3, 3, 0, 0.0f, 0.0f, 1.0f, 0.0f);
		vertexTwo = new CCoord(10, temp - 5, 0, 1.0f, 0.0f, 0.0f, 1.0f);
		blueSlider->insertVertex(vertexOne);
		blueSlider->insertVertex(vertexTwo);
	}

	blueSlider->draw();
	greenSlider->draw();
	redSlider->draw();

	screen_box(140, 0, ww - 140, wh - shift); //message box
	displayText(145, 80, messageLog, true);

	glPopAttrib();
}

// drawing callback
void display(void)
{
	// save rendering state
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)ww, 0.0, (GLdouble)wh, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, ww, wh);

	// draw the painting screen
	glClear(GL_COLOR_BUFFER_BIT);
	menu_area();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(140, ww, 100, wh - 20, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(140, 100, ww - 140, wh - 120);

	glPushMatrix();
	glScalef(ww / WIDTH, wh / HEIGHT, 1.0f);
	tempShape = shapes;
	while (tempShape)
	{
		tempShape->draw();
		tempShape = tempShape->nextShape;
	}
	glPopMatrix();

	if (rubberShape && rubberShape->good)
	{
		rubberShape->draw();
		delete rubberShape;
		rubberShape = NULL;
	}
	
	// restore state
	glPopAttrib();
	glutSwapBuffers();
}

std::ostream& operator<<(std::ostream& os, const CShape& shape)
{
	// write out individual members of s with an end of line between each one
	
	os << shape.type << ',';
	os << shape.fill << ',';
	os << shape.size << ',';
	os << '"' << shape.text << '"' << ',';
	os << shape.nbrVert << ',';	
	CCoord * temp = shape.vertHead;
	for (int i = 0; i < shape.nbrVert; i++) 
	{
		os << temp->x << ',' << temp->y << ',' << temp->z << ',' << temp->r << ',' << temp->g << ',' << temp->b << ',' << temp->a << ',';
		temp = temp->nextCoord;
	}
	os << '\n';
	return os;
}

std::istream& operator>>(std::istream& is, CShape& shape)
{
	// read in individual members of s
	is >> shape.type >> shape.fill >> shape.size >> shape.nbrVert >> shape.text;
	CCoord * temp = shape.vertHead;
	return is;
}

// setup window
int main(int argc, char** argv)
{
	int c_menu, f_menu, ps_menu, cl_menu;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize((int)WIDTH, (int)HEIGHT);
	glutCreateWindow("Complex Paint");
	glutDisplayFunc(display);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(key);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(rubberband);
	glutMotionFunc(motion);
	myinit();

	// create a menu for the colors
	c_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);
	glutAddMenuEntry("Current Color", 9);

	ps_menu = glutCreateMenu(point_size_menu);
	glutAddMenuEntry("1", 1);
	glutAddMenuEntry("2", 2);
	glutAddMenuEntry("3", 3);
	glutAddMenuEntry("4", 4);
	glutAddMenuEntry("5", 5);
	glutAddMenuEntry("6", 6);
	glutAddMenuEntry("7", 7);
	glutAddMenuEntry("8", 8);
	glutAddMenuEntry("9", 9);
	glutAddMenuEntry("10", 10);

	// create clear menu
	cl_menu = glutCreateMenu(clear_menu);
	glutAddMenuEntry("Clear last shape", 1);
	glutAddSubMenu("Set clear color", c_menu);
	glutAddMenuEntry("Clear screen", 2);

	// create a shape option menu
	f_menu = glutCreateMenu(fill_menu);
	glutAddMenuEntry("Fill on", 1);
	glutAddMenuEntry("Fill off", 2);

	// create main menu
	glutCreateMenu(right_menu);
	glutAddSubMenu("Pixel size", ps_menu);
	glutAddSubMenu("Fill", f_menu);
	glutAddSubMenu("Clear", cl_menu);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // attach menu to right button

	glutMainLoop();
}