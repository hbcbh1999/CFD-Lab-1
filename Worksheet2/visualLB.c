#include "visualLB.h"
#include <stdio.h>
#include "LBDefinitions.h"
#include "helper.h"
#include "computeCellValues.h"

void writeVtkOutput(const double * const collideField, const int * const flagField, const char * filename, unsigned int t, int xlength) {
    char fn[80];
    /* Save filename as a combination of passed filename and timestep */
    sprintf(fn, "%s.%i.vtk", filename, t);
    
    FILE *fp = fopen(fn, "w");
    if (fp == NULL) {
        ERROR("Failed to open file!");
        return;
    }
    
    fprintf(fp, "# vtk DataFile Version 2.0\n");
    fprintf(fp, "generated by CFD-lab course output \n");
    fprintf(fp, "ASCII\n\n");
    fprintf(fp, "DATASET STRUCTURED_GRID\n");
    fprintf(fp, "DIMENSIONS %d %d %d \n", xlength+1, xlength+1, xlength+1);
    fprintf(fp, "POINTS %d float\n\n", (xlength+1) * (xlength+1) * (xlength+1));
    /* to continue*/
