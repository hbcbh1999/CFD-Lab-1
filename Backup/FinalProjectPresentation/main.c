#include "helper.h"
#include "visual.h"
#include "init.h"
#include "sor.h"
#include "boundary_val.h"
#include "uvp.h"
#include <stdio.h>

int main(int argn, char** args){
	if (argn !=2 ) {
		printf("When running the simulation, please give a valid geometry file name!\n");
		return 1;
	}
	//set the geometry file
	char *filename = NULL;
	filename = args[1];

	//initialize variables
	double t = 0; /*time start*/
	int it, n = 0; /*iteration and time step counter*/
	double res; /*residual for SOR*/
	/*arrays*/
	double ***U, ***V, ***W, ***P;
	double ***RS, ***F, ***G, ***H;
	int ***Flag; //additional data structure for arbitrary geometry
	/*int matrix_output = 0;*/
	/*those to be read in from the input file*/
	double Re, UI, VI, WI, PI, GX, GY, GZ, t_end, xlength, ylength, zlength, dt, dx, dy, dz, alpha, omg, tau, eps, dt_value;
	int  imax, jmax, kmax, itermax;

	//double presLeft, presRight, presDelta; //for pressure stuff  ...TODO: not allowed for now
	int wl, wr, wt, wb, wf, wh;
	char problemGeometry[32];
	//in case of a given inflow or wall velocity  TODO: will we have this? needs to be a vector?
	double velIN;
	double velMW[3]; // the moving wall velocity is a vector

	//read the parameters, using problem.dat, including wl, wr, wt, wb
	read_parameters(filename, &Re, &UI, &VI, &WI, &PI, &GX, &GY, &GZ, &t_end, &xlength, &ylength, &zlength, &dt, &dx, &dy, &dz, &imax,
			&jmax, &kmax, &alpha, &omg, &tau, &itermax, &eps, &dt_value, &wl, &wr,  &wf, &wh, &wt, &wb, problemGeometry, &velIN, &velMW[0]); //&presLeft, &presRight, &presDelta, &vel);


	//printf("d: %f, %f, %f\n", dx,dy,dz);


	//int pics = dt_value/dt; //just a helping variable for outputing vtk
	double last_output_t = -dt_value;
	//double every = t_end/10; //helping variable. Can be used for displaying info every tenth of the progress during simulation

	//allocate memory, including Flag
	U = matrix2(0, imax+1, 0, jmax+1, 0, kmax+1);
	V = matrix2(0, imax+1, 0, jmax+1, 0, kmax+1);
	W = matrix2(0, imax+1, 0, jmax+1, 0, kmax+1);
	P = matrix2(0, imax+1, 0, jmax+1, 0, kmax+1);
	RS = matrix2(1, imax, 1, jmax, 1, kmax);

	F = matrix2(0, imax, 1, jmax, 1, kmax);
	G = matrix2(1, imax, 0, jmax, 1, kmax);
	H = matrix2(1, imax, 1, jmax, 0, kmax);


	Flag = imatrix2(0, imax+1, 0, jmax+1, 0, kmax+1); // or Flag = imatrix(1, imax, 1, jmax);

	//initialisation, including **Flag
	init_flag(problemGeometry, imax, jmax, kmax, Flag, wl, wr, wf, wh, wt, wb);  //presDelta, Flag);
	init_uvwp(UI, VI, WI, PI, Flag,imax, jmax, kmax, U, V, W, P, problemGeometry);


	//write_vtkFile("init", n, xlength, ylength, zlength, imax, jmax, kmax, dx, dy, dz, U, V, W, P);
	//going through all time steps

	while(t < t_end){
		/*if(t - every >= 0){
      	  	  printf("Calculating time %f ... \n", t);
      	  	  every += t;
    	}*/

		//adaptive time stepping
		calculate_dt(Re, tau, &dt, dx, dy, dz, imax, jmax, kmax, U, V, W);
		//    printf("calc dt \n");

		//setting bound.values
		boundaryvalues(imax, jmax, kmax, U, V, W, P, wl, wr, wf, wh, wt, wb, F, G, H, problemGeometry, Flag, velIN, velMW); //including P, wl, wr, wt, wb, F, G, problem
		//    printf("calc bc \n");

		//computing F, G, H and right hand side of pressue eq.
		calculate_fgh(Re, GX, GY, GZ, alpha, dt, dx, dy, dz, imax, jmax, kmax, U, V, W, F, G, H, Flag);
		//    printf("calc fgh \n");

		calculate_rs(dt, dx, dy, dz, imax, jmax, kmax, F, G, H, RS);
		//    printf("calc rs \n");

		//iteration counter
		it = 0;

		do{
			//
			//perform SOR iteration, at same time set bound.values for P and new residual value
			sor(omg, dx, dy, dz, imax, jmax, kmax, P, RS, &res, Flag); //, presLeft, presRight);

			it++;
		}while(it<itermax && res>eps);
		/*if (it == itermax) {
			printf("Warning: sor while loop exits because it reaches the itermax. res = %f, time =%f\n", res, t);
		} */

		//calculate U, V and W of this time step
		calculate_uvw(dt, dx, dy, dz, imax, jmax, kmax, U, V, W, F, G, H, P, Flag);
		//    printf("calc uvw \n");

		//indent time and number of time steps
		n++;
		t += dt;



		//output of pics for animation
		if ( t-last_output_t  >= dt_value ){  //n%pics==0 ){
			/*
			if(matrix_output%10==0){

				write_matrix2("U.txt",t,U, 0, imax+1, 0, jmax+1, 0, kmax+1);
				write_matrix2("V.txt",t,V, 0, imax+1, 0, jmax+1, 0, kmax+1);
				write_matrix2("W.txt",t,W, 0, imax+1, 0, jmax+1, 0, kmax+1);
				write_matrix2("P.txt",t,P, 0, imax+1, 0, jmax+1, 0, kmax+1);
				write_matrix2("RS.txt",t,RS, 1, imax, 1, jmax, 1, kmax);


				write_matrix2("F.txt",t,F, 0, imax, 1, jmax, 1, kmax);
				write_matrix2("G.txt",t,G, 1, imax, 0, jmax, 1, kmax);
				write_matrix2("H.txt",t,H, 1, imax, 1, jmax, 0, kmax);
			}
			*/

			write_vtkFile(filename, n, xlength, ylength, zlength, imax, jmax, kmax, dx, dy, dz, U, V, W, P,Flag);

			printf("output vtk (%d)\n",n);
			last_output_t = t;
			/*matrix_output++;*/
		}
		printf("timestep: %f   - next output: %f   (dt: %f) \n",t,dt_value- (t-last_output_t),dt);
	}

	//output of U, V, P at the end for visualization
	//write_vtkFile("DrivenCavity", n, xlength, ylength, imax, jmax, dx, dy, U, V, P);
	//free memory
	free_matrix2(U, 0, imax+1, 0, jmax+1, 0, kmax+1);
	free_matrix2(V, 0, imax+1, 0, jmax+1, 0, kmax+1);
	free_matrix2(W, 0, imax+1, 0, jmax+1, 0, kmax+1);
	free_matrix2(P, 0, imax+1, 0, jmax+1, 0, kmax+1);
	free_matrix2(RS, 1, imax, 1, jmax, 1, kmax);


	free_matrix2(F, 0, imax, 1, jmax, 1, kmax);
	free_matrix2(G, 1, imax, 0, jmax, 1, kmax);
	free_matrix2(H, 1, imax, 1, jmax,  0, kmax);


	free_imatrix2(Flag, 0, imax+1, 0, jmax+1, 0, kmax+1);

	//printf("\n-\n");
	return -1;
}
