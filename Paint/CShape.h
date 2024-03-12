/*
*  File: CShape.h
*  Authors: Kyle Loveless, Charlie Miller, and Cornelius Donley
___________                     ____   ____     .__               __
\__    ___/___ _____    _____   \   \ /   /____ |  |___  __ _____/  |______
  |    |_/ __ \\__  \  /     \   \   Y   // __ \|  |\  \/ // __ \   __\__  \
  |    |\  ___/ / __ \|  Y Y  \   \     /\  ___/|  |_\   /\  ___/|  |  / __ \_
  |____| \___  >____  /__|_|  /    \___/  \___  >____/\_/  \___  >__| (____  /
             \/     \/      \/                \/               \/          \/
*/

// File CShape.h
// Author: S. Renk 10/07
// Shape classes that form a graphics library for library for a Paint program.

#ifndef CSHAPE_H_
#define CSHAPE_H_

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <string>

// define constants for all shapes
#define CPOINT 0
#define CLINE 1
#define CCIRCLE 2
#define CRECT 3
#define CPOLY 4
#define CTRI 5
#define CARC 6
#define CTEXT 7

// ********************** CCord class ********************
class CCoord // basic vertex class
{
public:
	int x, y, z; // vertex cartesian coord (z=0 for 2D)
	float r, g, b, a; // vertex color (a=1.0f)
	CCoord * nextCoord; // ptr. to next vertex in a linked list 
	CCoord(int nx = 0, int ny = 0, int nz = 0, float nr = 0, float ng = 0, float nb = 0, float na = 1.0f)
	{
		nextCoord = NULL; x = nx; y = ny; z = nz; r = nr; g = ng; b = nb; a = na;
	}
};

// ********************* CShape class ********************
class CShape // abstract base class for all Shapes
{
protected:
	bool CShape::insert(CCoord * vert) // links in a vertex
	{
		if (!vertHead) vertHead = vertTail = vert;
		else
		{
			vertTail->nextCoord = vert;
			vertTail = vert;
		}
		nbrVert++;
		return true;
	}

public:
	int type; // 0- point, 1-line, ...
	bool good;
	int nbrVert; // # vertices, if = 0 bad figure
	bool fill;
	int size;
	std::string text;
	CCoord * vertHead, *vertTail; // list of vertices in figure
	CShape * nextShape; // points to next shape in list
	CShape(int ntype) { vertHead = vertTail = NULL; nextShape = NULL; nbrVert = 0; type = ntype; good = false; }
	~CShape() // delete linked list of verts
	{
		CCoord * temp = vertHead;
		while (temp)
		{
			vertHead = vertHead->nextCoord;
			delete temp;
			temp = vertHead;
		}
	}
	static void insertShape(CShape * &shape, CShape * &newShape)
	{
		CShape * temp = shape;
		if (!shape) shape = newShape;
		else
		{
			while (temp->nextShape != NULL)
				temp = temp->nextShape;
			temp->nextShape = newShape;
		}
	}
	
	virtual void draw() = 0; //Draws the figure 
	virtual bool isPicked(int x, int y, int z = 0) = 0; // true if (x,y,z) is inside figure
	virtual bool insertVertex(CCoord * vert) = 0; // place in vertex list
	// virtual void mouseDrag(int x, int y, int state) = 0; // mouse drag for rubberbanding
	// virtual read() = 0; // read object from file -- I'd actually overload << & >>
	// virtual write() = 0; // write object to file
};

// ******************** CPoint class ********************
class CPoint : public CShape // A list of points
{
public:
	CPoint() :CShape(CPOINT) {} // pass type to base
	void draw()	// draw a set of points 
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPointSize((GLfloat)size);
		glBegin(GL_POINTS);
		glColor4f(vertHead->r, vertHead->g, vertHead->b, vertHead->a);
		glVertex3i(vertHead->x, vertHead->y, vertHead->z);
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex) { CShape::insert(vertex); return good = true; }
	//void mouseDrag(int x, int y, int state){} // do nothing
};

// ******************** CLine class *********************
class CLine : public CShape // A list of lines. Every pair is a line.
{
public:
	CLine() :CShape(CLINE) {} // pass type to base
	void draw()      // draw a set of lines
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glLineWidth((GLfloat)size);
		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);	
		for (CCoord * ptr = vertHead; ptr; ptr = ptr->nextCoord)
		{
			glColor4f(ptr->r, ptr->g, ptr->b, ptr->a);
			glVertex3i(ptr->x, ptr->y, ptr->z);
		}
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex)
	{
		CShape::insert(vertex);
		return good = (nbrVert > 1);
	}
	//void mouseDrag(int x, int y, int state); //mouse drag function
};

// ******************* CCircle class ********************
class CCircle : public CShape // A single circle
{
protected:
	double radius;

public:
	CCircle() :CShape(CCIRCLE) {} // pass type to base
	void draw() // draw a set of lines 
	{
		int x, y;
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor4f(vertHead->r, vertHead->g, vertHead->b, vertHead->a);
		if (fill)
		{
			glBegin(GL_TRIANGLE_FAN);
			x = vertHead->x, y = vertHead->y;
			glVertex3i(x, y, vertHead->z);
		}
		else
			glBegin(GL_LINE_STRIP);
		for (double rad = 0; rad <= 2.0 * 3.15; rad += 0.01)
		{			
			x = vertHead->x + (int)(radius*cos(rad));
			y = vertHead->y + (int)(radius*sin(rad));
			glVertex3i(x, y, vertHead->z);
		}
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex)
	{
		if (nbrVert == 0)
			CShape::insert(vertex);
		else
		{
			int x = vertex->x - vertHead->x;
			int y = vertex->y - vertHead->y;
			radius = sqrt((double)(x * x + y * y));
			good = true;
			CShape::insert(vertex);
		}
		return good;
	}
};

// ***************** CRect class ***********************
class CRect : public CShape
{
public:
	CRect() :CShape(CRECT) {};
	void draw()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);
		glColor4f(vertHead->r, vertHead->g, vertHead->b, vertHead->a);
		glVertex3i(vertHead->x, vertHead->y, vertHead->z);
		glVertex3i(vertHead->x, vertHead->nextCoord->y, vertHead->z);
		glVertex3i(vertHead->nextCoord->x, vertHead->nextCoord->y, vertHead->z);
		glVertex3i(vertHead->nextCoord->x, vertHead->y, vertHead->z);
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) 
	{ 
		//printf("Mouse X: %i, Mouse Y: %i\nHead X: %i Head: Y: %i\nTail X: %i Tail Y: %i\n", x, y, vertHead->x, vertHead->y, vertTail->x, vertTail->y);
		if (x >= vertHead->x && x <= vertTail->x && y >= vertHead->y && y <= vertTail->y)
			return true;
		return false;
	}
	bool insertVertex(CCoord * vertex)
	{
		CShape::insert(vertex);
		return good = (nbrVert > 1);
	}
	//void mouseDrag(int x, int y, int state); //mouse drag function
};

// ***************** CPoly class ***********************
class CPoly : public CShape
{
public:
	CPoly() :CShape(CPOLY) {};
	void draw()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LINE_SMOOTH | GL_POLYGON_SMOOTH);
		if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POLYGON);
		for (CCoord * ptr = vertHead; ptr; ptr = ptr->nextCoord)
		{
			glColor4f(ptr->r, ptr->g, ptr->b, ptr->a);
			glVertex3i(ptr->x, ptr->y, ptr->z);
		}
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex)
	{
		CShape::insert(vertex);
		return good = (nbrVert > 2);
	}
	//void mouseDrag(int x, int y, int state); //mouse drag function
};

// ***************** CTri class ***********************
class CTri : public CShape
{
public:
	CTri() :CShape(CTRI) {};
	void draw()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_LINE_SMOOTH | GL_POLYGON_SMOOTH);
		if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
		for (CCoord * ptr = vertHead; ptr; ptr = ptr->nextCoord)
		{
			glColor4f(ptr->r, ptr->g, ptr->b, ptr->a);
			glVertex3i(ptr->x, ptr->y, ptr->z);
		}
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex)
	{
		CShape::insert(vertex);
		return good = (nbrVert > 2);
	}
	//void mouseDrag(int x, int y, int state); //mouse drag function
};

// ******************* CArc class ********************
class CArc : public CShape // A single circle
{
protected:
	double radius;

public:
	CArc() :CShape(CARC) {} // pass type to base
	void draw() // draw a set of lines 
	{
		int x, y;
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor4f(vertHead->r, vertHead->g, vertHead->b, vertHead->a);
		if (fill)
		{
			glBegin(GL_TRIANGLE_FAN);
			x = vertHead->x, y = vertHead->y;
			glVertex3i(x, y, vertHead->z);
		}
		else
			glBegin(GL_LINE_STRIP);		
		double theta = atan2(vertHead->nextCoord->y - vertHead->y, vertHead->nextCoord->x - vertHead->x);
		double theta2 = atan2(vertHead->nextCoord->nextCoord->y - vertHead->y, vertHead->nextCoord->nextCoord->x - vertHead->x);
		if (theta < 0) theta += 2.0 * 3.14159;
		if (theta2 < 0) theta2 += 2.0 * 3.14159;
		if(theta > theta2) theta2 += 2.0 * 3.14159;
		if(size)
			for (double rad = theta; rad <= theta2; rad += 0.01)
			{
				x = vertHead->x + (int)(radius*cos(rad));
				y = vertHead->y + (int)(radius*sin(rad));
				glVertex3i(x, y, vertHead->z);
			}
		else
			for (double rad = theta2; rad <= theta + 2 * 3.14159; rad += 0.01)
			{
				x = vertHead->x + (int)(radius*cos(rad));
				y = vertHead->y + (int)(radius*sin(rad));
				glVertex3i(x, y, vertHead->z);
			}
		glEnd();
		glPopAttrib();
	}
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex)
	{		
		CShape::insert(vertex);		
		if (nbrVert == 2)
		{
			int x = vertHead->nextCoord->x - vertHead->x;
			int y = vertHead->nextCoord->y - vertHead->y;
			radius = sqrt((double)(x * x + y * y));
		}
		return (good = nbrVert > 2);
	}
};

// ***************** CText class ***********************
class CText : public CShape
{	
public:
	CText() : CShape(CTEXT){}
	void draw()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor4f(vertHead->r, vertHead->g, vertHead->b, vertHead->a);
		glRasterPos2i(vertHead->x, vertHead->y);
		for (std::string::iterator i = text.begin(); i != text.end(); ++i)
		{
			char c = *i;
			if (c == '\n')
			{
				c = *++++i;
			}
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
		}
		glPopAttrib();
	}
	
	bool isPicked(int x, int y, int z = 0) { return false; }
	bool insertVertex(CCoord * vertex) { CShape::insert(vertex); return good = true; }
};
#endif
