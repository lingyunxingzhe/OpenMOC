/*
 * Surface.cpp
 *
 *  Created on: Jan 9, 2012
 *      Author: wbinventor
 */

#include "Surface.h"

/* _n keeps track of the number of surfaces instantiated */
int Surface::_n = 0;


/**
 * Default Surface constructor
 * @param id the surface id
 * @param type the surface type
 * @param boundary this surface's boundary type
 */
Surface::Surface(const int id, const surfaceType type,
					const boundaryType boundary){
	_uid = _n;
	_id = id;
	_type = type;
	_n++;
	_boundary = boundary;
}


/**
 * Surface Destructor
 */
Surface::~Surface() { }


/**
 * Return the surface's uid
 * @return the surface's uid
 */
int Surface::getUid() const {
	return _uid;
}

/**
 * Return the surface's id
 * @return the surface's id
 */
int Surface::getId() const {
    return _id;
}


/**
 * Return the surface's type
 * @return the surface type
 */
surfaceType Surface::getType() const {
    return _type;
}


/**
 * Returns the vector of the surface ids on the positive side of this surface
 * @return vector of surface ids
 */
std::vector<Cell*> Surface::getNeighborPos() {
	return _neighbor_pos;
}


/**
 * Returns the vector of the surface ids on the negative side of this surface
 * @return vector of surface id
 */
std::vector<Cell*> Surface::getNeighborNeg() {
	return _neighbor_neg;
}


/**
 * Allocates memory for a certain number of neighbor cells on
 * the positive side of this surface
 * @param size the number of cells
 */
void Surface::setNeighborPosSize(int size) {
	try {
		_neighbor_pos.resize(size);
	}
	catch (std::exception &e) {
		log_printf(ERROR, "Could not resize a vector for the positive"
				" neighbor cells for surface id = %d. Backtrace:\n%s",
				_id, e.what());
	}
}


/**
 * Allocates memory for a certain number of neighbor cells on
 * the negative side of this surface
 * @param size the number of cells
 */
void Surface::setNeighborNegSize(int size) {
	try {
		_neighbor_neg.resize(size);
	}
	catch (std::exception &e) {
		log_printf(ERROR, "Could not resize a vector for the positive"
				" neighbor cells for surface id = %d. Backtrace:\n%s",
				_id, e.what());
	}
}


/**
 * Sets the id for a neighboring cell on the positive side
 * of this surface
 * @param index the index of the neighbor cell
 * @param cell the cell id
 */
void Surface::setNeighborPos(int index, Cell* cell) {
	_neighbor_pos[index] = cell;
}


/**
 * Sets the id for a neighboring cell on the negative side
 * of this surface
 * @param index the index of the neighbor cell
 * @param cell the cell id
 */
void Surface::setNeighborNeg(int index, Cell* cell) {
	_neighbor_neg[index] = cell;
}


/**
 * Returns the surface's boundary type
 * @return the boundary type (REFLECTIVE or BOUNDARY_NONE)
 */
boundaryType Surface::getBoundary(){
       return _boundary;
}


/**
 * Return true or false if a point is on or off of a surface.
 * @param point pointer to the point of interest
 * @return true (on) or false (off)
 */
bool Surface::onSurface(Point* point) {

	/* Uses a threshold to determine whether the point is on the surface */
	if (abs(evaluate(point)) < ON_SURFACE_THRESH)
		return true;
	else
		return false;
}


/**
 * Return true or false if a localcoord is on or off of a surface
 * @param point pointer to the localcoord of interest
 * @return true (on) or false (off)
 */
bool Surface::onSurface(LocalCoords* coord) {
	return onSurface(coord->getPoint());
}


/**
 * Finds the minimum distance from a point with a given trajectory defined
 * by an angle to this surface. If the trajectory will not intersect the
 * surface, returns INFINITY
 * @param point a pointer to the point of interest
 * @param angle the angle defining the trajectory in radians
 * @param intersection a pointer to a point for storing the intersection
 * @return the minimum distance
 */
double Surface::getMinDistance(Point* point, double angle,
									Point* intersection) {

	/* Point array for intersections with this surface */
	Point intersections[2];

	/* Find the intersection point(s) */
	int num_inters = this->intersection(point, angle, intersections);
	double distance = INFINITY;

	/* If there is one intersection point */
	if (num_inters == 1) {
		distance = intersections[0].distance(point);
		intersection->setX(intersections[0].getX());
		intersection->setY(intersections[0].getY());
	}

	/* If there are two intersection points */
	else if (num_inters == 2) {
		double dist1 = intersections[0].distance(point);
		double dist2 = intersections[1].distance(point);

		/* Determine which intersection point is nearest */
		if (dist1 < dist2) {
			distance = dist1;

			intersection->setX(intersections[0].getX());
			intersection->setY(intersections[0].getY());
		}
		else {
			distance = dist2;
			intersection->setX(intersections[1].getX());
			intersection->setY(intersections[1].getY());
		}
	}

	return distance;
}



/**
 * Plane constructor
 * @param id the surface id
 * @param boundary this surface's boundary type
 * @param A the first coefficient in A * x + B * y + C = 0;
 * @param B the second coefficient in A * x + B * y + C = 0;
 * @param C the third coefficient in A * x + B * y + C = 0;
 */
Plane::Plane(const int id, const boundaryType boundary,
	     const double A, const double B,
	     const double C): Surface(id, PLANE, boundary) {
	_A = A;
	_B = B;
	_C = C;
}


/**
 * Evaluate a point using the plane's quadratic surface equation
 * @param point a pointer to the point of interest
 * @return the value of point in the equation
 */
double Plane::evaluate(const Point* point) const {
	double x = point->getX();
	double y = point->getY();
	return (_A * x + _B * y + _C);
}


/**
 * Converts this Plane's attributes to a character array
 * @param a character array of this plane's attributes
 */
std::string Plane::toString() {
	std::stringstream string;

	string << "Surface id = " << _id << ", type = PLANE " << ", A = "
			<< _A << ", B = " << _B << ", C = " << _C;

	return string.str();
}



/**
 * Finds the intersection point with this plane from a given point and
 * trajectory defined by an angle
 * @param point pointer to the point of interest
 * @param angle the angle defining the trajectory in radians
 * @param points pointer to a point to store the intersection point
 * @return the number of intersection points (0 or 1)
 */
int Plane::intersection(Point* point, double angle, Point* points) {

	double x0 = point->getX();
	double y0 = point->getY();

	int num = 0; 			/* number of intersections */
	double xcurr, ycurr;	/* coordinates of current intersection point */

	/* The track is vertical */
	if ((fabs(angle - (M_PI / 2))) < 1.0e-10) {

		/* The plane is also vertical => no intersections */
		if (_B == 0)
			return 0;

		/* The plane is not vertical */
		else {
			xcurr = x0;
			ycurr = (-_A * x0 - _C) / _B;
			points->setCoords(xcurr, ycurr);
			/* Check that point is in same direction as angle */
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;
			return num;
		}
	}

	/* If the track isn't vertical */
	else {
		double m = sin(angle) / cos(angle);

		/* The plane and track are parallel, no intersections */
		if (fabs(-_A/_B - m) < 1e-11 && _B != 0)
			return 0;

		else {
			xcurr = -(_B * (y0 - m * x0) + _C)
					/ (_A + _B * m);
			ycurr = y0 + m * (xcurr - x0);
			points->setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;

			return num;
		}
	}
}


/**
 * Returns the minimum x value on this surface
 * @return the minimum x value
 */
double Plane::getXMin(){
	log_printf(ERROR, "Plane::getXMin not implemented");
	return -1.0/0.0;
}


/**
 * Returns the maximum x value on this surface
 * @return the maximum x value
 */
double Plane::getXMax(){
	log_printf(ERROR, "Plane::getXMax not implemented");
	return 1.0/0.0;
}


/**
 * Returns the minimum y value on this surface
 * @return the minimum y value
 */
double Plane::getYMin(){
	log_printf(ERROR, "Plane::getYMin not implemented");
	return -1.0/0.0;
}


/**
 * Returns the maximum y value on this surface
 * @return the maximum y value
 */
double Plane::getYMax(){
	log_printf(ERROR, "Plane::getYMax not implemented");
	return 1.0/0.0;
}

/**
 * XPlane constructor for a plane parallel to the x-axis
 * @param id the surface id
 * @param boundary this surface's boundary type
 * @param C the location of the plane along the y-axis
 */
XPlane::XPlane(const int id, const boundaryType boundary,
		const double C): Plane(id, boundary, 0, 1, -C) {
	_type = XPLANE;
}


/**
 * Converts this XPlane's attributes to a character array
 * @param a character array of this plane's attributes
 */
std::string XPlane::toString() {
	std::stringstream string;

	string << "Surface id = " << _id << ", type = XPLANE " << ", A = "
			<< _A << ", B = " << _B << ", C = " << _C;

	return string.str();
}


/**
 * Returns the minimum x value on this surface
 * @return the minimum x value
 */
double XPlane::getXMin(){
	return -_C;
}


/**
 * Returns the maximum x value on this surface
 * @return the maximum x value
 */
double XPlane::getXMax(){
	return -_C;
}


/**
 * Returns the minimum y value on this surface
 * @return the minimum y value
 */
double XPlane::getYMin(){
	return 1.0/0.0;
}


/**
 * Returns the maximum y value on this surface
 * @return the maximum y value
 */
double XPlane::getYMax(){
	return -1.0/0.0;
}


/**
 * YPlane constructor for a plane parallel to the y-axis
 * @param id the surface id
 * @param boundary the surface boundary type
 * @param C the location of the plane along the x-axis
 */
YPlane::YPlane(const int id, const boundaryType boundary,
		const double C): Plane(id, boundary, 1, 0, -C) {
	_type = YPLANE;
}



/**
 * Converts this YPlane's attributes to a character array
 * @param a character array of this plane's attributes
 */
std::string YPlane::toString() {
	std::stringstream string;

	string << "Surface id = " << _id << ", type = YPLANE " << ", A = "
			<< _A << ", B = " << _B << ", C = " << _C;

	return string.str();
}


/**
 * Returns the minimum x value on this surface
 * @return the minimum x value
 */
double YPlane::getXMin(){
	return 1.0/0.0;
}


/**
 * Returns the maximum x value on this surface
 * @return the maximum x value
 */
double YPlane::getXMax(){
	return -1.0/0.0;
}


/**
 * Returns the minimum y value on this surface
 * @return the minimum y value
 */
double YPlane::getYMin(){
	return -_C;
}

/**
 * Returns the maximum y value on this surface
 * @return the maximum y value
 */
double YPlane::getYMax(){
	return -_C;
}


/**
 * Circle constructor
 * @param id the surface id
 * @param boundary the surface boundary type
 * @param x the x-coordinte of the circle center
 * @param y the y-coordinate of the circle center
 * @param radius the radius of the circle
 */
Circle::Circle(const int id, const boundaryType boundary, const double x,
		const double y, const double radius): Surface(id, CIRCLE, boundary) {
	_A = 1;
	_B = 1;
	_C = -2*x;
	_D = -2*y;
	_E = x*x + y*y - radius*radius;
	_radius = radius;
	center.setX(x);
	center.setY(y);
}

/**
 * Return the radius of the circle
 * @return the radius of the circle
 */
double Circle::getScale() {
	return this->_radius;
}



/**
 * Evaluate a point using the circle's quadratic surface equation
 * @param point a pointer to the point of interest
 * @return the value of point in the equation
 */
double Circle::evaluate(const Point* point) const {
	double x = point->getX();
	double y = point->getY();
	return (_A * x * x + _B * y * y + _C * x + _D * y + _E);
}



/**
 * Finds the intersection point with this circle from a given point and
 * trajectory defined by an angle (0, 1, or 2 points)
 * @param point pointer to the point of interest
 * @param angle the angle defining the trajectory in radians
 * @param points pointer to a an array of points to store intersection points
 * @return the number of intersection points (0 or 1)
 */
int Circle::intersection(Point* point, double angle, Point* points) {

	double x0 = point->getX();
	double y0 = point->getY();
	double xcurr, ycurr;
	int num = 0;			/* Number of intersection points */
	double a, b, c, q, discr;

	/* If the track is vertical */
	if ((fabs(angle - (M_PI / 2))) < 1.0e-10) {
		/* Solve for where the line x = x0 and the surface F(x,y) intersect
		 * Find the y where F(x0, y) = 0
		 * Substitute x0 into F(x,y) and rearrange to put in
		 * the form of the quadratic formula: ay^2 + by + c = 0
		 */
		a = _B * _B;
		b = _D;
		c = _A * x0 * x0 + _C * x0 + _E;

		discr = b*b - 4*a*c;

		/* There are no intersections */
		if (discr < 0)
			return 0;

		/* There is one intersection (ie on the surface) */
		else if (discr == 0) {
			xcurr = x0;
			ycurr = -b / (2*a);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;
			return num;
		}

		/* There are two intersections */
		else {
			xcurr = x0;
			ycurr = (-b + sqrt(discr)) / (2 * a);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;

			xcurr = x0;
			ycurr = (-b - sqrt(discr)) / (2 * a);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;
			return num;
		}
	}

	/* If the track isn't vertical */
	else {
		/*Solve for where the line y-y0 = m*(x-x0) and the surface F(x,y) intersect
		 * Find the (x,y) where F(x, y0 + m*(x-x0)) = 0
		 * Substitute the point-slope formula for y into F(x,y) and rearrange to put in
		 * the form of the quadratic formula: ax^2 + bx + c = 0
		 * double m = sin(track->getPhi()) / cos(track->getPhi());
		 */
		double m = sin(angle) / cos(angle);
		q = y0 - m * x0;
		a = _A + _B * _B * m * m;
		b = 2 * _B * m * q + _C + _D * m;
		c = _B * q * q + _D * q + _E;

		discr = b*b - 4*a*c;

		/* There are no intersections */
		if (discr < 0)
			return 0;

		/* There is one intersection (ie on the surface) */
		else if (discr == 0) {
			xcurr = -b / (2*a);
			ycurr = y0 + m * (points[0].getX() - x0);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0)
				num++;
			else if (angle > M_PI && ycurr < y0)
				num++;
			return num;
		}

		/* There are two intersections */
		else {
			xcurr = (-b + sqrt(discr)) / (2*a);
			ycurr = y0 + m * (xcurr - x0);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0) {
				num++;
			}
			else if (angle > M_PI && ycurr < y0) {
				num++;
			}

			xcurr = (-b - sqrt(discr)) / (2*a);
			ycurr = y0 + m * (xcurr - x0);
			points[num].setCoords(xcurr, ycurr);
			if (angle < M_PI && ycurr > y0) {
				num++;
			}
			else if (angle > M_PI && ycurr < y0) {
				num++;
			}

			return num;
		}
	}
}


/**
 * Converts this Plane's attributes to a character array
 * @param a character array of this plane's attributes
 */
std::string Circle::toString() {
	std::stringstream string;

	string << "Surface id = " << _id << ", type = CIRCLE " << ", A = "
			<< _A << ", B = " << _B << ", C = " << _C << ", D = " << _D
			<< ", E = " << _E;

	return string.str();
}

double Circle::getXMin(){
	log_printf(ERROR, "Circle::getXMin not implemented");
	return -1.0/0.0;
}

double Circle::getXMax(){
	log_printf(ERROR, "Circle::getXMax not implemented");
	return 1.0/0.0;
}

double Circle::getYMin(){
	log_printf(ERROR, "Circle::getYMin not implemented");
	return -1.0/0.0;
}

double Circle::getYMax(){
	log_printf(ERROR, "Circle::getYMax not implemented");
	return 1.0/0.0;
}


Cruciform::Cruciform(const int id, const boundaryType boundary,
    const double _x, const double _y, 
    const double _scale, const double _rotation): Surface(id, CRUCIFORM, boundary) {
    x = _x;
    y = _y;
	scale = _scale;
    rotation = _rotation;
}

Cruciform::~Cruciform() {

#ifdef CRUCIMEMOIZE
    // Delete our memoized intersection map
    unordered_map<long, vector<Point*>* >::iterator it;
    int j;
    for(it=memintersections.begin(); it!=memintersections.end(); ++it)
    {
        for(j=0; j<(int)it->second->size(); j++)
            delete it->second->at(j);
        delete it->second;
    }
#endif
}

double resqrt(double x) {
    return x >= 0 ? sqrt(x) : 0;
}

/**
 * Evaluate a point using the circle's quadratic surface equation
 * @param point a pointer to the point of interest
 * @return the value of point in the equation
 */

double Cruciform::evaluate(const Point* point) const {
    double rx = (point->getX() - x)/scale;
    double ry = (point->getY() - y)/scale;
    return cruci(rx, ry, rotation);
}

double Cruciform::cruci(double px, double py, double rot) {
    double uax = px*cos(-rot) - py*sin(-rot);
    double uay = px*sin(-rot) + py*cos(-rot);

	double ax = abs(uax);
	double ay = abs(uay);

    // I've added a term so that we don't go to zero beyond x = 0.719...

    double ret =  ay - (
        (ax <= 0.18585) * (resqrt(0.03454 - pow(ax, 2)) + 0.5334) +
        (ax > 0.18585) * (ax <= 0.5334) * (0.5334 - resqrt(0.120756 - pow(ax - 0.5334, 2))) +
        (ax > 0.5334) * (ax < 0.719249402) * resqrt(0.03454 - pow(ax - 0.5334, 2)) +
        (ax >= 0.719249402) * (ay - resqrt(pow(ax,2) + pow(ay,2)) + 0.719249402));
    return ret;
}


Point* Cruciform::scalarsecant(double x_n, double x_nm1, Point* initial, double angle) {
    double x0 = (initial->getX() - x) / scale;
    double y0 = (initial->getY() - y) / scale;
    double delx = cos(angle);
    double dely = sin(angle);

    double y_n, y_nm1, dx;
    int max_iterations = 100;

    y_nm1 = cruci(x_nm1 * delx + x0, x_nm1 * dely + y0, rotation);
    y_n   = cruci(x_n * delx + x0, x_n * dely + y0, rotation);

    if (abs(y_n) > abs(y_nm1)){
        swap(x_nm1, x_n);
        swap(y_nm1, y_n);
    }

    for(int iters=0; iters<max_iterations; iters++) {
        if(y_n == y_nm1)
            return (Point*)0;
        dx = y_n * (x_n - x_nm1) / (y_n - y_nm1);
        if(abs(y_n) < EPSILON && abs(x_n - x_nm1) < EPSILON) {
            if(x_n < 2 && x_n > 0)
                return new Point(
                    x + scale * (x_n * delx + x0),
                    y + scale * (x_n * dely + y0));
            else
                return (Point*)0;
        }
        x_nm1 = x_n;
        x_n = x_n - dx;
        y_nm1 = y_n;
        y_n = cruci(x_n * delx + x0, x_n * dely + y0, rotation);
    }

    return (Point*)0;
}

// See if any of the approximations of previous points match
long hash_point_angle(Point* point, double angle) {
    long x = (long)abs(floor(point->getX() * MATCHSHIFT));
    long y = (long)abs(floor(point->getY() * MATCHSHIFT));
    long a = (long)abs(floor(angle * MATCHSHIFT));
    long xs = (long)(point->getX() > 0.0);
    long ys = (long)(point->getY() > 0.0);
    return ((2*xs + ys) << (3*MATCHBITS)) |
        (a << (MATCHBITS*2)) | (x << MATCHBITS) | y;
}

/**
 * Finds the intersection point with this circle from a given point and
 * trajectory defined by an angle (0, 1, or 2 points)
 * @param point pointer to the point of interest
 * @param angle the angle defining the trajectory in radians
 * @param points pointer to a an array of points to store intersection points
 * @return the number of intersection points (0 or 1)
 */
int Cruciform::intersection(Point* point, double angle, Point* points) {
    double xn, xnm1;
    long curhash;
    Point* cur;
    vector<Point*>* intersections = new vector<Point*>();
    int num = 0;

    unordered_set<long> matches;
    long curargs = hash_point_angle(point, angle);

#ifdef CRUCIMEMOIZE
    if(memintersections[curargs]) {
        intersections = memintersections[curargs];
    } else {
#endif
        matches.insert(curargs);

        for(double xx=0.1; xx <= 1.0; xx += 0.3)
        {
            xn = xx - 0.05;
            xnm1 = xx + 0.05;
            cur = scalarsecant(xn, xnm1, point, angle);

            if(!cur)
                continue;

            curhash = hash_point_angle(cur, angle);

            if (matches.count(curhash)) {
                delete cur;
                continue;
            }

            matches.insert(curhash);
            intersections->push_back(cur);
        }
#ifdef CRUCIMEMOIZE
        memintersections[curargs] = intersections;
    }
#endif

    // I think this function should really be returning a vector,
    // but I'll use this minor boilerplate to deal with this
    int mnum = min(2, (int)intersections->size());
    
    for(num=0; num < mnum; num++) 
        points[num].setCoords(intersections->at(num)->getX(),
            intersections->at(num)->getY());
    return mnum;
}

/**
 * Converts this Plane's attributes to a character array
 * @param a character array of this plane's attributes
 */
string Cruciform::toString() {
	stringstream string;

	string << "Surface type = CRUCIFORM " << " x = "
			<< x << ", y = " << y << ", x0 = " << center.getX() 
			<< ", y0 = " << center.getY() << ", scale = " << scale;

	return string.str();
}


double Cruciform::getXMin(){
	log_printf(ERROR, "Cruciform::getXMin not implemented");
	return -1.0/0.0;
}

double Cruciform::getXMax(){
	log_printf(ERROR, "Cruciform::getXMax not implemented");
	return 1.0/0.0;
}

double Cruciform::getYMin(){
	log_printf(ERROR, "Cruciform::getYMin not implemented");
	return -1.0/0.0;
}

double Cruciform::getYMax(){
	log_printf(ERROR, "Cruciform::getYMax not implemented");
	return 1.0/0.0;
}

double Cruciform::getScale() {
    return scale;
}
