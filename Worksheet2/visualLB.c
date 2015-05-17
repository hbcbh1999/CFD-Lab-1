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
   
    /* Print lattice points */
    for (int x = 1; x < xlength + 2; ++x) {
        for (int y = 1; y < xlength + 2; ++y) {
            for (int z = 1; z < xlength + 2; ++z) {
                fprintf(fp, "%d %d %d\n", x, y, z);
            }
        }
    }
    
    fprintf(fp, "POINT_DATA %d \n\n", (xlength+1) * (xlength+1) * (xlength+1));
    fprintf(fp, "VECTORS velocity float\n");
    
    double density;
    double vel[D];
    const double *currentCell;
    /* Compute (macroscopic) velocities for all cells */
    for (int x = 1; x < xlength+2; ++x) {
        for (int y = 1; y < xlength+2; ++y) {
            for (int z = 1; z < xlength+2; ++z) {
                currentCell = &collideField + Q*(z*xlength*xlength+y*xlength+x);/*with or without"&"??*/
                computeDensity(currentCell, &density);
                computeVelocity(currentCell, &density, vel);
                fprintf(fp, "%f %f %f\n", vel[0], vel[1], vel[2]);
            }
        }
    }
    
    fprintf(fp, "\nCELL_DATA %d \n", (xlength) * (xlength) * (xlength));
    fprintf(fp, "SCALARS density float 1 \n");
    fprintf(fp, "LOOKUP_TABLE default \n");
    
    /* Compute density for each cell */
    for (int x = 1; x < xlength+1; ++x) {
        for (int y = 1; y < xlength+1; ++y) {
            for (int z = 1; z < xlength+1; ++z) {
                currentCell = &collideField + Q*(z*xlength*xlength+y*xlength+x);/*with or without"&"??*/
                computeDensity(currentCell, &density);
                fprintf(fp, "%f\n", density);
            }
        }
    }
    
    if (fclose(fp)) {
        ERROR("Failed to close file!");
        return;
    }
}
