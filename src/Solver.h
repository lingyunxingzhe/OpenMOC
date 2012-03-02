/*
 * Solver.h
 *
 *  Created on: Feb 7, 2012
 *      Author: William Boyd
 *				MIT, Course 22
 *              wboyd@mit.edu
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <utility>
#include <math.h>
#include <unordered_map>
#include <limits.h>
#include <string>
#include <sstream>
#include "Geometry.h"
#include "Quadrature.h"
#include "Track.h"
#include "TrackGenerator.h"
#include "FlatSourceRegion.h"
#include "configurations.h"
#include "log.h"

class Solver {
private:
	Geometry* _geom;
	Quadrature* _quad;
	FlatSourceRegion* _flat_source_regions;
	Track** _tracks;
	int* _num_tracks;
	int _num_azim;
	int _num_FSRs;
	double* _FSRs_to_fluxes[NUM_ENERGY_GROUPS + 1];
	double _k_eff;
	double _k_eff_old;
	Plotter* _plotter;
	float* _pix_map_total_flux;
	bool _plot_fluxes;
	int* _pix_map_FSRs;
#if !STORE_PREFACTORS
	double* _pre_Factor_Array;
	int _array_size;
	double _precision;
#endif
	void precomputeFactors();
	double computePreFactor(segment* seg, int energy, int angle);
	void initializeFSRs();
public:
	Solver(Geometry* geom, TrackGenerator* track_generator, Plotter* plotter, bool plotFluxes);
	virtual ~Solver();
	void zeroTrackFluxes();
	void oneFSRFluxes();
	void zeroFSRFluxes();
	void computeRatios();
	void updateKeff();
	double** getFSRtoFluxMap();
	void fixedSourceIteration(int max_iterations);
	double computeKeff(int max_iterations);
	void plotFluxes();
};

#endif /* SOLVER_H_ */
