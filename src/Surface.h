/*
 * Surface.h
 *
 *  Created on: Jan 9, 2012
 *      Author: wbinventor
 */

#ifndef SURFACE_H_
#define SURFACE_H_

#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include "Point.h"
#include "Track.h"
#include "Cell.h"
#include "LocalCoords.h"

#define EPSILON 1E-6
#define MATCHSHIFT 1E4
#define MATCHBITS 16

class LocalCoords;

#include "log.h"

/* Define for compiler */
class Plane;
class Cell;

/**
 * Surface types
 */
enum surfaceType {
	PLANE,
	CIRCLE,
    CRUCIFORM,
	XPLANE,
	YPLANE,
	QUADRATIC
};


/**
 * Surface boundary types
 */
enum boundaryType {
	BOUNDARY_NONE,
	REFLECTIVE
};

using namespace std;

/**
 * Represents a 2-dimensional quadratics surface
 */
class Surface {
protected:
	static int _n;		  /* Counts the number of surfaces */
	int _uid;		  /* monotonically increasing id based on n */
	int _id;
	surfaceType _type;
	boundaryType _boundary;
	vector<Cell*> _neighbor_pos;
	vector<Cell*> _neighbor_neg;

public:
	Surface(const int id, const surfaceType type, 
		const boundaryType boundary);
	virtual ~Surface();
	int getUid() const;
	int getId() const;
	surfaceType getType() const;
	vector<Cell*> getNeighborPos();
	vector<Cell*> getNeighborNeg();
	void setNeighborPosSize(int size);
	void setNeighborNegSize(int size);
	void setNeighborPos(int index, Cell* cell);
	void setNeighborNeg(int index, Cell* cell);
	boundaryType getBoundary();
	virtual double evaluate(const Point* point) const =0;
	virtual int intersection(Point* point, double angle, Point* points) =0;
	virtual string toString() =0;
	virtual double getXMin() =0;
	virtual double getXMax() =0;
	virtual double getYMin() =0;
	virtual double getYMax() =0;
    virtual double getScale() {return 0;};
	bool onSurface(Point* point);
	bool onSurface(LocalCoords* coord);
	double getMinDistance(Point* point, double angle, Point* intersection);
};


/**
 * Represents a plane in 2D as a Surface subclass
 */
class Plane: public Surface {
protected:
	double _A, _B, _C;
	friend class Surface;
	friend class Circle;
public:
	Plane(const int id, const boundaryType boundary, const double A, const double B, const double C);
	double evaluate(const Point* point) const;
	int intersection(Point* point, double angle, Point* points);
	string toString();
	virtual double getXMin();
	virtual double getXMax();
	virtual double getYMin();
	virtual double getYMax();
};


/**
 * Represents a plane parallel to the x-axis as a Plane subclass
 */
class XPlane: public Plane {
private:
public:
	XPlane(const int id, const boundaryType boundary, const double C);
	string toString();
	virtual double getXMin();
	virtual double getXMax();
	virtual double getYMin();
	virtual double getYMax();
};

/**
 * Represents a plane parallel to the y-axis as a Plane subclass
 */
class YPlane: public Plane {
private:
public:
	YPlane(const int id, const boundaryType boundary, const double C);
	string toString();
	virtual double getXMin();
	virtual double getXMax();
	virtual double getYMin();
	virtual double getYMax();
};

/**
 * Represents a circle as a Surface subclass
 */
class Circle: public Surface {
private:
	Point center;
	double _radius;
	double _A, _B, _C, _D, _E;
	friend class Surface;
	friend class Plane;
public:
	Circle(const int id, const boundaryType boundary, const double x,
				const double y, const double radius);
	double evaluate(const Point* point) const;
	int intersection(Point* point, double angle, Point* points);
	string toString();
	virtual double getXMin();
	virtual double getXMax();
	virtual double getYMin();
	virtual double getYMax();
    double getScale();
};

class Cruciform: public Surface {
private:
	Point center;
    double x, y;
	double scale;
    double rotation;
    unordered_map<long, vector<Point*>* > memintersections;
	friend class Surface;
	friend class Plane;
public:
	Cruciform(const int id, const boundaryType boundary, const double _x,
		const double _y, const double _scale, const double _rotation);
    ~Cruciform();
	double evaluate(const Point* point) const;
    static double cruci(double px, double py, double rot);
    Point* scalarsecant(double x_n, double x_nm1, Point* initial, double angle);
	int intersection(Point* point, double angle, Point* points);
	string toString();

	virtual double getXMin();
	virtual double getXMax();
	virtual double getYMin();
	virtual double getYMax();
    double getScale();
};


#endif /* SURFACE_H_ */
