#include "helper.h"
#include "visual.h"
#include <stdio.h>
#include <stdlib.h>



double getValidValue(double v){
	if( isnan(v)){//check for NaN
		return -1;
	}
	else if(isinf(v)){//check for Infinity
		return -2;
	}
	else{
		return v;
	}
}


void write_particles_debug(const char *szProblem, int    timeStepNumber, int N, struct particleline *Partlines){
	int k;
	struct particle *p;
	double x,y,z;

	char szFileName[80];
	FILE *fp=NULL;
///media/norbert/940CB6150CB5F27A/Documents/
	sprintf( szFileName, "simulation/%s.%i.csv", szProblem, timeStepNumber );
	fp = fopen( szFileName, "w");
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to open %s", szFileName );
		ERROR( szBuff );
		return;
	}

	fprintf(fp,"x,y,z,v,t\n");

	for(k=0;k<N;k++){
		for(p=Partlines[k].Particles; p->next != NULL; p=p->next){
			x = p->next->x;
			y = p->next->y;
			z = p->next->z;
			fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
		}
	}

	if( fclose(fp) )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to close %s", szFileName );
		ERROR( szBuff );
	}

}



void write_vtkFile_debug(const char *szProblem,
		int    timeStepNumber,
		double xlength,
		double ylength,
		double zlength,
		int    imax,
		int    jmax,
		int    kmax,
		double dx,
		double dy,
		double dz,
		double ***U,
		double ***V,
		double ***W,
		double ***P,
		int ***Flag) {

	int i,j,k;
	double uVel,vVel,wVel;
	char szFileName[80];
	FILE *fp=NULL;
	///media/norbert/940CB6150CB5F27A/Documents/
	//sprintf( szFileName, "simulation/%s.%i_debug.vtk", szProblem, timeStepNumber );
	///media/norbert/940CB6150CB5F27A/Documents/
	sprintf( szFileName, "simulation/%s.%i_debug.vtk", szProblem, timeStepNumber );

	fp = fopen( szFileName, "w");
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to open %s", szFileName );
		ERROR( szBuff );
		return;
	}

	write_vtkHeader_debug(fp, imax, jmax, kmax, dx, dy, dz);
	write_vtkPointCoordinates_debug(fp, imax, jmax, kmax, dx, dy, dz);


	fprintf(fp,"POINT_DATA %i \n", (imax+3)*(jmax+3)*(kmax+3) );

	fprintf(fp,"\n");
	fprintf(fp, "VECTORS velocity float\n");
	for(k = -1; k < kmax+2; k++) {
		for(j = -1; j < jmax+2; j++) {
			for(i = -1; i < imax+2; i++) {
				if(i >= 0 && j >= 0 && k >= 0 && i < imax+1 && j < jmax+1 && k < kmax+1){
					uVel = getValidValue((U[i][j][k] + U[i][j+1][k] + U[i][j][k+1] + U[i][j+1][k+1]) * 0.25);
					vVel = getValidValue((V[i][j][k] + V[i+1][j][k] + V[i][j][k+1] + V[i+1][j][k+1]) * 0.25);
					wVel = getValidValue((W[i][j][k] + W[i+1][j][k] + W[i][j+1][k] + W[i+1][j+1][k]) * 0.25);
					fprintf(fp, "%f %f %f\n",uVel,vVel,wVel);
				}
				else{
					fprintf(fp, "0 0 0\n");
				}
			}
		}
	}

	fprintf(fp,"\n");
	fprintf(fp,"CELL_DATA %i \n", ((imax+2)*(jmax+2)*(kmax+2)) );
	fprintf(fp, "SCALARS pressure float 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");
	for(k = 0; k < kmax+2; k++) {
		for(j = 0; j < jmax+2; j++) {
			for(i = 0; i < imax+2; i++) {
				if(P[i][j][k]==0){
					fprintf(fp, "-5\n");
				}
				else
				{
					fprintf(fp, "%f\n",getValidValue(P[i][j][k]));
				}
			}
		}
	}


	fprintf(fp,"\n");
	fprintf(fp, "SCALARS flag short 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");

	for(k = 0; k < kmax+2; k++) {
		for(j = 0; j < jmax+2; j++) {
			for(i = 0; i < imax+2; i++) {



				if(isfluid(Flag[i][j][k])){

					if(issurface(Flag[i][j][k])){
						fprintf(fp, "2\n");
					}
					else{
						fprintf(fp, "1\n");
					}
				}
				else if(isboundary(Flag[i][j][k])){
					//printf("boundary: %d",getboundarytype(Flag[i  ][j  ][k  ]));
					fprintf(fp, "3\n");
				}
				else
					fprintf(fp, "0\n");

				/*
				if(isboundary(Flag[i][j][k])){
					//printf("boundary: (%d,%d,%d) %d, %d -> %d\n",i,j,k,Flag[i  ][j  ][k  ],getcelltype(Flag[i  ][j  ][k  ]),getboundarytype(Flag[i  ][j  ][k  ]));

					fprintf(fp, "%d\n",getboundarytype(Flag[i  ][j  ][k  ]));
				}
				else
					fprintf(fp, "0\n");



				if(isboundary(Flag[i][j][k])){
					//printf("boundary: (%d,%d,%d) %d, %d -> %d\n",i,j,k,Flag[i  ][j  ][k  ],getcelltype(Flag[i  ][j  ][k  ]),getboundarytype(Flag[i  ][j  ][k  ]));

					fprintf(fp, "%d\n",getboundarytype(Flag[i  ][j  ][k  ]));
				}
				else
					fprintf(fp, "0\n");


				  fprintf(fp, "%d\n",getcelltype(Flag[i][j][k]));

				 */
			}
		}
	}

	if( fclose(fp) )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to close %s", szFileName );
		ERROR( szBuff );
	}
}


void write_vtkHeader_debug( FILE *fp, int imax, int jmax, int kmax,
		double dx, double dy, double dz) {
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Null pointer in write_vtkHeader" );
		ERROR( szBuff );
		return;
	}

	fprintf(fp,"# vtk DataFile Version 2.0\n");
	fprintf(fp,"generated by CFD-lab course output \n");
	fprintf(fp,"ASCII\n");
	fprintf(fp,"\n");
	fprintf(fp,"DATASET STRUCTURED_GRID\n");
	fprintf(fp,"DIMENSIONS  %i %i %i \n", imax+3, jmax+3, kmax+3);
	fprintf(fp,"POINTS %i float\n", (imax+3)*(jmax+3)*(kmax+3) );
	fprintf(fp,"\n");
}


void write_vtkPointCoordinates_debug( FILE *fp, int imax, int jmax, int kmax,
		double dx, double dy, double dz) {
	/*  double originX = 0.0;
  double originY = 0.0;
	double originZ = 0.0;
	 */
	double i, j, k;
	for(k = 0; k < kmax+3; k+=1) {
		for(j = 0; j < jmax+3; j+=1) {
			for(i = 0; i < imax+3; i+=1) {
				fprintf(fp, "%f %f %f\n", i*dx, j*dy, k*dz );
			}
		}
	}
}



/*#############################################################*/
#define WRITE_PARTICLE_PROB 10

void write_particles(const char *szProblem, int    timeStepNumber, double dx,double dy,double dz, int N, struct particleline *Partlines,int ***Flag){

	struct particle *p;
	double x,y,z;

	char szFileName[80];
	FILE *fp=NULL;

	//sprintf( szFileName, "/media/norbert/940CB6150CB5F27A/Documents/simulation/%s.%i.csv", szProblem, timeStepNumber );
	sprintf( szFileName, "simulation/%s.%i.csv", szProblem, timeStepNumber );
	fp = fopen( szFileName, "w");
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to open %s", szFileName );
		ERROR( szBuff );
		return;
	}
	unsigned int iseed = (unsigned int)time(NULL);
	  srand (iseed);
	fprintf(fp,"x,y,z,v,t\n");
	int i,j,k;
	//int k;
	double r = 0;
	//int c = 0;
	for(k=0;k<N;k++){
		for(p=Partlines[k].Particles; p->next != NULL; p=p->next){
			x = p->next->x;
			y = p->next->y;
			z = p->next->z;

			i = (int)(x/dx)+1;

			j = (int)(y/dy)+1;

			k = (int)(z/dz)+1;

			if(issurface(Flag[i][j][k])||nonfluidneighbor(Flag[i][j][k])){
				fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
			}
			else
			{
				r = (rand ()*100.0)/(RAND_MAX*1.0);
				if(r< WRITE_PARTICLE_PROB){
					fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
				}

			}


			/*
			r = (rand ()*100.0)/(RAND_MAX*1.0);
			if(r< WRITE_PARTICLE_PROB){
				fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
			}

			if(c % WRITE_PARTICLE_PROB == 0){
				fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
			}

			if(issurface(Flag[i][j][k])){
				fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
			}
			else{
				r = (rand ()*100.0)/(RAND_MAX*1.0);
				if(r< WRITE_PARTICLE_PROB){
					fprintf(fp,"%f,%f,%f,%f,1\n",x,y,z, p->next->vel);
				}

			}
			 */
			//c++;
		}
	}

	if( fclose(fp) )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to close %s", szFileName );
		ERROR( szBuff );
	}

}





void write_vtkFile(const char *szProblem,
		int    timeStepNumber,
		double xlength,
		double ylength,
		double zlength,
		int    imax,
		int    jmax,
		int    kmax,
		double dx,
		double dy,
		double dz,
		double ***U,
		double ***V,
		double ***W,
		double ***P,
		int ***Flag) {

	int i,j,k;
	double uVel,vVel,wVel;
	char szFileName[80];
	FILE *fp=NULL;
	//sprintf( szFileName, "simulation/%s.%i.vtk", szProblem, timeStepNumber );
	sprintf( szFileName, "simulation/%s.%i.vtk", szProblem, timeStepNumber );

	fp = fopen( szFileName, "w");
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to open %s", szFileName );
		ERROR( szBuff );
		return;
	}

	write_vtkHeader(fp, imax, jmax, kmax, dx, dy, dz);
	write_vtkPointCoordinates(fp, imax, jmax, kmax, dx, dy, dz);

	fprintf(fp,"POINT_DATA %i \n", (imax+1)*(jmax+1)*(kmax+1) );

	fprintf(fp,"\n");
	fprintf(fp, "VECTORS velocity float\n");


	double yoold = -111111;
	//printf("--------------------\n");
	for(k = 0; k < kmax+1; k++) {
		for(j = 0; j < jmax+1; j++) {
			for(i = 0; i < imax+1; i++) {
				//fprintf(fp, "%f %f %f\n", getValidValue((U[i][j][k] + U[i+1][j][k]) * 0.5), getValidValue((V[i][j][k] + V[i][j+1][k]) * 0.5), getValidValue((W[i][j][k] + W[i][j][k+1]) * 0.5) );
				//fprintf(fp, "%f %f %f\n", getValidValue((U[i][j][k] + U[i][j+1][k]) * 0.5), getValidValue((V[i][j][k] + V[i+1][j][k]) * 0.5), getValidValue((W[i][j][k] + W[i][j][k+1]) * 0.5) );
				uVel = getValidValue((U[i][j][k] + U[i][j+1][k] + U[i][j][k+1] + U[i][j+1][k+1]) * 0.25);
				vVel = getValidValue((V[i][j][k] + V[i+1][j][k] + V[i][j][k+1] + V[i+1][j][k+1]) * 0.25);
				wVel = getValidValue((W[i][j][k] + W[i+1][j][k] + W[i][j+1][k] + W[i+1][j+1][k]) * 0.25);
				if(yoold!=V[i][j][k]){
					yoold =V[i][j][k];
					//printf("vel (%d,%d,%d): %f\n",i,j,k,yoold);
				}
				fprintf(fp, "%f %f %f\n",uVel,vVel,wVel);
			}
		}
	}
	//printf( "%f %f %f\n",uVel,vVel,wVel);
	fprintf(fp,"\n");
	fprintf(fp,"CELL_DATA %i \n", ((imax)*(jmax)*(kmax)) );
	fprintf(fp, "SCALARS pressure float 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");
	double poold = -111111;

	for(k = 1; k < kmax+1; k++) {
		for(j = 1; j < jmax+1; j++) {
			for(i = 1; i < imax+1; i++) {
				if(P[i][j][k]==0){
					fprintf(fp, "-5\n");
				}
				else
				{
					fprintf(fp, "%f\n",getValidValue(P[i][j][k]));
				}
				if(poold!=P[i][j][k]){
					poold =P[i][j][k];
					//printf("p (%d,%d,%d): %f\n",i,j,k,poold);
				}
			}
		}
	}


	fprintf(fp,"\n");
	fprintf(fp, "SCALARS flag short 1 \n");
	fprintf(fp, "LOOKUP_TABLE default \n");



	int nx=2,ny=2,nz=2;
	int ui=0,vi=0,wi=0;
	int num = 0,type=0,xdb=0,ydb=0,zdb=0;

	i = nx*ny*nz*ui*vi*wi*num*type*xdb*ydb*zdb; //todo remove



	for(k = 1; k < kmax+1; k++) {
		for(j = 1; j < jmax+1; j++) {
			for(i = 1; i < imax+1; i++) {


				if(isfluid(Flag[i][j][k])){

					if(issurface(Flag[i][j][k])){
						fprintf(fp, "2\n");
					}
					else{
						fprintf(fp, "1\n");
					}
				}
				else if(isboundary(Flag[i][j][k])){
					//printf("boundary: %d",getboundarytype(Flag[i  ][j  ][k  ]));
					fprintf(fp, "3\n");
				}
				else
					fprintf(fp, "0\n");





				/*
				 *
				 *
fprintf(fp, "%d\n",isfluid(Flag[i][j][k]) && !emptyneighbor(Flag[i][j][k]));

				if(issurface(Flag[i][j][k])&&isfluid(Flag[i][j][k])){
					type = getsurfacetype(Flag[i][j][k],&ui,&vi,&wi,&nx,&ny,&nz,&xdb,&ydb,&zdb,&num);
				    fprintf(fp, "%d\n",type);
				   // printf("s!!!!!!!!!!!!!!!! %d\n",type);
				}
				else{
					 fprintf(fp, "0\n");
				}
				 fprintf(fp, "%d\n",emptyneighbor(Flag[i][j][k]));

				if(issurface(Flag[i][j][k]) && isfluid(Flag[i][j][k])){

					fprintf(fp, "20\n");
				}
				else{

				}
				 */
			}
		}
	}

	if( fclose(fp) )
	{
		char szBuff[80];
		sprintf( szBuff, "Failed to close %s", szFileName );
		ERROR( szBuff );
	}
}


void write_vtkHeader( FILE *fp, int imax, int jmax, int kmax,
		double dx, double dy, double dz) {
	if( fp == NULL )
	{
		char szBuff[80];
		sprintf( szBuff, "Null pointer in write_vtkHeader" );
		ERROR( szBuff );
		return;
	}

	fprintf(fp,"# vtk DataFile Version 2.0\n");
	fprintf(fp,"generated by CFD-lab course output \n");
	fprintf(fp,"ASCII\n");
	fprintf(fp,"\n");
	fprintf(fp,"DATASET STRUCTURED_GRID\n");
	fprintf(fp,"DIMENSIONS  %i %i %i \n", imax+1, jmax+1, kmax+1);
	fprintf(fp,"POINTS %i float\n", (imax+1)*(jmax+1)*(kmax+1) );
	fprintf(fp,"\n");
}


void write_vtkPointCoordinates( FILE *fp, int imax, int jmax, int kmax,
		double dx, double dy, double dz) {
	/*  double originX = 0.0;
  double originY = 0.0;
	double originZ = 0.0;
	 */
	double i, j, k;
	for(k = 0; k < kmax+1; k+=1) {
		for(j = 0; j < jmax+1; j+=1) {
			for(i = 0; i < imax+1; i+=1) {
				fprintf(fp, "%f %f %f\n", i*dx, j*dy, k*dz );
			}
		}
	}
}

