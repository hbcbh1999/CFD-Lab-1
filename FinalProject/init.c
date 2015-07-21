#include "helper.h"
#include "init.h"
#include <math.h>
#include "boundary_val.h"

#define DROP_CUBE 1
#define DROP_SPHERE_1 2
#define DROP_SPHERE_2 3


#define DROP_TYPE DROP_CUBE

#define DROP_RADIUS 3
#define DROP_WATER_HEIGHT 10

#define BREAKING_DAM_WIDTH 20


int read_parameters( const char *szFileName,       /* name of the file */
		double *Re,                /* reynolds number   */
		double *UI,                /* velocity x-direction */
		double *VI,                /* velocity y-direction */
		double *WI,                /* velocity z-direction */
		double *PI,                /* pressure */
		double *GX,                /* gravitation x-direction */
		double *GY,                /* gravitation y-direction */
		double *GZ,                /* gravitation z-direction */
		double *t_end,             /* end time */
		double *xlength,           /* length of the domain x-dir.*/
		double *ylength,           /* length of the domain y-dir.*/
		double *zlength,           /* length of the domain z-dir.*/
		double *dt,                /* time step */
		double *dx,                /* length of a cell x-dir. */
		double *dy,                /* length of a cell y-dir. */
		double *dz,                /* length of a cell z-dir. */
		int  *imax,                /* number of cells x-direction*/
		int  *jmax,                /* number of cells y-direction*/
		int  *kmax,                /* number of cells z-direction*/
		double *alpha,             /* uppwind differencing factor*/
		double *omg,               /* relaxation factor */
		double *tau,               /* safety factor for time step*/
		int  *itermax,             /* max. number of iterations for pressure per time step */
		double *eps,               /* accuracy bound for pressure*/
		double *dt_value,          /* time for output */
		int *wl,              		 /*initial boundary for left wall*/
		int *wr,              		 /*initial boundary for right wall*/
		int *wf,		               /*initial boundary for front wall*/
		int *wh,			             /*initial boundary for back wall*/
		int *wt,			             /*initial boundary for top wall*/
		int *wb,			             /*initial boundary for bottom wall*/
		char *problemGeometry,     /*problem to solve*/
		//                  double *presLeft,		       /*pressure at the left wall*/
		//                  double *presRight,		     /*pressure at the right wall*/
		//                  double *presDelta,		     /*pressure difference across the domain*/
		double *velIN,             /*velocity of inflow*/
		double *velMW,
		int *particles)		         /*velocity of wall (in U direction)*/
{
	READ_DOUBLE( szFileName, *xlength );
	READ_DOUBLE( szFileName, *ylength );
	READ_DOUBLE( szFileName, *zlength );

	READ_INT   ( szFileName, *imax );
	READ_INT   ( szFileName, *jmax );
	READ_INT   ( szFileName, *kmax );

	READ_DOUBLE( szFileName, *dt    );
	READ_DOUBLE( szFileName, *t_end );
	READ_DOUBLE( szFileName, *tau   );

	READ_DOUBLE( szFileName, *dt_value );
	READ_INT   ( szFileName, *itermax );
	READ_INT   ( szFileName, *particles );

	READ_DOUBLE( szFileName, *eps   );
	READ_DOUBLE( szFileName, *omg   );
	READ_DOUBLE( szFileName, *alpha );

	READ_DOUBLE( szFileName, *Re    );

	READ_DOUBLE( szFileName, *GX );
	READ_DOUBLE( szFileName, *GY );
	READ_DOUBLE( szFileName, *GZ );

	READ_DOUBLE( szFileName, *PI );
	READ_DOUBLE( szFileName, *UI );
	READ_DOUBLE( szFileName, *VI );
	READ_DOUBLE( szFileName, *WI );

	READ_INT   ( szFileName, *wl );
	READ_INT   ( szFileName, *wr );
	READ_INT   ( szFileName, *wf );
	READ_INT   ( szFileName, *wh );
	READ_INT   ( szFileName, *wt );
	READ_INT   ( szFileName, *wb );

	READ_STRING( szFileName, problemGeometry );

	/*   READ_DOUBLE( szFileName, *presLeft);
   READ_DOUBLE( szFileName, *presRight);
   READ_DOUBLE( szFileName, *presDelta); */

	READ_DOUBLE( szFileName, *velIN );

	double *velMWx = &velMW[0], *velMWy = &velMW[1], *velMWz = &velMW[2];
	READ_DOUBLE( szFileName, *velMWx );
	READ_DOUBLE( szFileName, *velMWy );
	READ_DOUBLE( szFileName, *velMWz );
	/*velMW[0] = velMWx;
   velMW[1] = velMWy;
   velMW[2] = velMWz;*/

	//take care of (in)valid pressure input
	/*if (*presDelta<=0){
   //    if (fmin(presLeft, presRight)<0): we dont have pressure input
	if  (fmin(*presLeft, *presRight)>=0){
	 *presDelta = *presLeft - *presRight;
        }
   } else { //deltaP is given
	if  (*presLeft< *presDelta){
		if (*presRight<0){
	 *presLeft = *presDelta;
	 *presRight = 0.0;
		} else {
	 *presLeft = *presRight + *presDelta;
		}
	} else {//pressure on left wall is also given
	 *presRight = *presLeft - *presDelta;
	}
   }
   if (*presDelta>0){ // if pressure given, left and right bound. set to outflow (they probably already are set in the input file, but just in case)
	 *wl = 3;
	 *wr = 3;
}*/
	*dx = *xlength / (double)(*imax);
	*dy = *ylength / (double)(*jmax);
	*dz = *zlength / (double)(*kmax);
	return 1;
}


/**
 * The arrays U,V,W and P are initialized to the constant values UI, VI, WI and PI on
 * the whole domain.
 */


void init_particles(int ***Flag,double dx,double dy,double dz,int imax,int jmax,int kmax,int ppc,struct particleline *Partlines,char * problem){
	int i,j,k;
	int pi = 0;
	double xd,yd,zd;
	Partlines[pi].length = 0;
	Partlines[pi].Particles = create_particle(-1,-1,-1);
	int ic,jc,kc;

	if(strcmp (problem,"drop.pgm")==0 && DROP_TYPE == DROP_SPHERE_1){
		double x,y,z;
		double mx = imax*0.5;
		double my = jmax*0.5+DROP_RADIUS-4;
		double mz = kmax*0.5;
		for(i = 1; i < imax+1; i++) {
			for(k = 1; k < kmax+1; k++) {
				for(j = 1; j < jmax+1; j++) {
					for (ic=1;ic<=ppc;ic++){
						x = (i-1)*dx+(ic-0.5)/((double)ppc)*dx;
						for (jc=1;jc<=ppc;jc++)
						{
							y = (j-1)*dy+(jc-0.5)/((double)ppc)*dy;
							for (kc=1;kc<=ppc;kc++)
							{
								z = (k-1)*dz+(kc-0.5)/((double)ppc)*dz;
								xd = fabs(x/dx-mx);
								yd = fabs(y/dy-my);
								zd = fabs(z/dz-mz);
								if(sqrt(xd*xd+yd*yd+zd*zd)<=DROP_RADIUS){
									add_particle(&Partlines[pi],x,y,z);
								}
							}
						}
					}
				}
			}
		}
		for(i = 0; i <= imax+1; i++) {
			for(j = 0; j < DROP_WATER_HEIGHT; j++) {
				for(k = 0; k <= kmax+1; k++) {
					if(isfluid(Flag[i][j][k])){
						add_cell_particles(&Partlines[pi], i, j, k,dx,dy,dz,ppc);
					}
				}
			}
		}
	}
	else
	{
		for(i = 0; i <= imax+1; i++) {
			for(j = 0; j <= jmax+1; j++) {
				for(k = 0; k <= kmax+1; k++) {
					if(isfluid(Flag[i][j][k])){
						add_cell_particles(&Partlines[pi], i, j, k,dx,dy,dz,ppc);
					}
				}
			}
		}
	}

}



void init_uvwp(
		double UI,
		double VI,
		double WI,
		double PI,
		int ***Flag,
		int imax,
		int jmax,
		int kmax,
		double ***U,
		double ***V,
		double ***W,
		double ***P,
		char * problem
) {
	int i,j,k;
	init_matrix2(U, 0, imax+1, 0, jmax+1, 0, kmax+1, UI);
	init_matrix2(V, 0, imax+1, 0, jmax+1, 0, kmax+1, VI);
	init_matrix2(W, 0, imax+1, 0, jmax+1, 0, kmax+1, WI);
	init_matrix2(P, 0, imax+1, 0, jmax+1, 0, kmax+1, PI);
	for(i = 0; i <= imax+1; i++) {
		for(j = 0; j <= jmax+1; j++) {
			for(k = 0; k <= kmax+1; k++) {
				if(!isfluid(Flag[i][j][k])){
					U[i  ][j  ][k  ] = 0;
					if(i>0)
						U[i-1][j  ][k  ] = 0;
					V[i  ][j  ][k  ] = 0;
					if(j>0)
						V[i  ][j-1][k  ] = 0;
					W[i  ][j  ][k  ] = 0;
					if(k>0)
						W[i  ][j  ][k-1] = 0;
					P[i  ][j  ][k  ] = 0;
				}
			}
		}
	}


	if(strcmp (problem,"step.pgm")==0 || strcmp (problem,"surface.pgm")==0){
		for(i = 0; i <= imax+1; i++) {
			for(j = 0; j <= jmax+1; j++) {
				for(k = 0; k <= kmax*0.5; k++) {
					U[i][j][k] = U[i][j][k] *0.5;
				}
			}
		}
	}
	else if(strcmp (problem,"drop.pgm")==0){
		for(i = 0; i <= imax+1; i++) {
			for(j = 0; j < DROP_WATER_HEIGHT; j++) {
				for(k = 0; k <= kmax+1; k++){

					V[i  ][j  ][k  ] = 0;
				}
			}
		}
	}


}



void init_cell(int ***Flag,int i, int j, int k ){
	int *flag=0,*flagc=0;
	flagc = &Flag[i][j  ][k  ];
	flag = &Flag[i+1][j  ][k  ];
	if(isempty(*flag)){  //B_O
		changebit(flagc,10,0);
		changebit(flagc,11,1);
	}
	else if (isfluid(*flag)){
		changebit(flagc,10,1);
		changebit(flagc,11,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,10,1);
		changebit(flagc,11,1);
	}
	flag = &Flag[i-1][j  ][k  ];
	if(isempty(*flag)){
		changebit(flagc,8,0); //B_W
		changebit(flagc,9,1);
	}
	else if (isfluid(*flag)){
		changebit(flagc,8,1);
		changebit(flagc,9,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,8,1);
		changebit(flagc,9,1);
	}
	flag = &Flag[i][j+1][k  ];
	if(isempty(*flag)){
		//printf("markcell!\n");
		changebit(flagc,6,0); //B_N
		changebit(flagc,7,1);
	}
	else if(isfluid(*flag)){
		changebit(flagc,6,1);
		changebit(flagc,7,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,6,1);
		changebit(flagc,7,1);
	}
	flag = &Flag[i][j-1][k  ];
	if(isempty(*flag)){
		//printf("markcell!\n");
		changebit(flagc,4,0); //B_S
		changebit(flagc,5,1);
	}
	else if(isfluid(*flag)){
		changebit(flagc,4,1);
		changebit(flagc,5,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,4,1);
		changebit(flagc,5,1);
	}
	flag = &Flag[i][j  ][k-1];
	if(isempty(*flag)){
		changebit(flagc,2,0); //B_D
		changebit(flagc,3,1);
	}
	else if (isfluid(*flag)){
		changebit(flagc,2,1);
		changebit(flagc,3,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,2,1);
		changebit(flagc,3,1);
	}
	flag = &Flag[i][j  ][k+1];
	if(isempty(*flag)){
		//printf("markcell!\n");
		changebit(flagc,0,0);//B_U
		changebit(flagc,1,1);
	}
	else if (isfluid(*flag)){
		changebit(flagc,0,1);
		changebit(flagc,1,0);
	}
	else if(isboundary(*flag)){
		changebit(flagc,0,1);
		changebit(flagc,1,1);
	}
}



void  init_sphere(
		char *problem,
		int imax,
		int jmax,
		int kmax,
		//  double presDelta,
		int ***Flag,
		int wl,
		int wr,
		int wf,
		int wh,
		int wt,
		int wb
) {
	int i,j,k;





	double dx=0,dy=0,dz=0;
	double mx = imax*0.5;
	double my = jmax*0.5;//-DROP_RADIUS-2;
	double mz = kmax*0.5;


	for(i = 1; i < imax+1; i++) {
		for(k = 1; k < kmax+1; k++) {
			for(j = 1; j < jmax+1; j++) {

				dx = fabs(i+0.5-mx);
				dy = fabs(j+0.5-my);
				dz = fabs(k+0.5-mz);



				if(DROP_TYPE == DROP_CUBE && dx <=DROP_RADIUS && dy <=DROP_RADIUS*1.1 && dz<=DROP_RADIUS){
					setcelltype(&Flag[i][j][k],FLUID);

				}
				else if(DROP_TYPE == DROP_SPHERE_2 && sqrt(dx*dx+dy*dy+dz*dz)<=DROP_RADIUS){
					setcelltype(&Flag[i][j][k],FLUID);

				}
				else if(j < DROP_WATER_HEIGHT){
					setcelltype(&Flag[i][j][k],FLUID);
				}

				else{
					setcelltype(&Flag[i][j][k],AIR);
				}


			}
		}


	}
}


void  init_breaking_dam(
		char *problem,
		int imax,
		int jmax,
		int kmax,
		//  double presDelta,
		int ***Flag,
		int wl,
		int wr,
		int wf,
		int wh,
		int wt,
		int wb
) {
	int i,j,k;
	for(i = 1; i < imax+1; i++) {
		for(k = 1; k < kmax+1; k++) {
			for(j = 1; j < jmax+1; j++) {
				if(i<BREAKING_DAM_WIDTH && j < jmax-5){
					setcelltype(&Flag[i][j][k],FLUID);
				}
				else{
					setcelltype(&Flag[i][j][k],AIR);
				}
			}
		}
	}

}




/**
 * The integer array Flag is initialized to constants C_F for fluid cells and C_B
 * for obstacle cells as specified by the parameter problem.
 */


void init_flag(
		char *problem,
		int imax,
		int jmax,
		int kmax,
		//  double presDelta,
		int ***Flag,
		int wl,
		int wr,
		int wf,
		int wh,
		int wt,
		int wb
) {
	int i,j,k;


	if(strcmp (problem,"drop.pgm")==0 || strcmp (problem,"breaking_dam.pgm")==0 ){
		for(i = 0; i <= imax+1; i++) {
			for(k = 0; k <= kmax+1; k++) {
				for(j = 0; j <= jmax+1; j++) {
					setcelltype(&Flag[i][j][k],FLUID);
				}
			}
		}

	}
	else{
		//read the geometry
		int **Pic = read_pgm(problem);
		for (k=1; k<kmax+1; k++) {
			//initialisation to C_F and C_B  Flag[i][0][0]
			for (int j=1; j<jmax+1; j++){ //i is the line, j the column. so left, right is j-1,j+1, and north, south is i-1, i+1. up/down (bigger/smaller z) is -/+ (ymax+2)*k
				for (int i=1; i<imax+1; i++){ //in Pic, 1 is where it's fluid, and 0 where it's air, apart from that: different boundary cond.
					Flag[i][jmax+1-j][k] = createflag(Pic,i,j,k,imax,jmax,kmax);
				}
			} //da bo to delal more bit slika tk narjena, da ma vsaka 2D podslika okoliinokoli ghost boundary. (s poljubno boundary cifro, tj med 2 in 6)
		}
	}


	//set outer boundary vortices, so you wont use them uninitialised. used getwallbit() bcs theyre on the edges of domain, so have only 2 or 3 neighbs, all boundary
	for (k=0; k<kmax+2; k++) {
		Flag[0][0][k] = getwallbit(wf);            //xz0
		Flag[0][jmax+1][k] = getwallbit(wh);       //xz1
		Flag[imax+1][0][k] = getwallbit(wf);       //xz0
		Flag[imax+1][jmax+1][k] = getwallbit(wh);  //xz1
	}
	for (j=0; j<jmax+2; j++) {
		Flag[0][j][0] = getwallbit(wb);            //xy0
		Flag[0][j][kmax+1] = getwallbit(wt);       //xy1
		Flag[imax+1][j][0] = getwallbit(wb);       //xy0
		Flag[imax+1][j][kmax+1] = getwallbit(wt);  //xy1
	}
	for (i=0; i<imax+2; i++) {
		Flag[i][0][0] = getwallbit(wf);            //xy0
		Flag[i][jmax+1][0] = getwallbit(wh);       //xy0
		Flag[i][0][kmax+1] = getwallbit(wf);       //xy1
		Flag[i][jmax+1][kmax+1] = getwallbit(wh);  //xy1
	}


	//set outer boundary flags - top and bottom:
	for (i=1; i<=imax; i++){
		for (j=1; j<=jmax; j++){
			if ((Flag[i][j][1]&pow2(2,12))==(Flag[i][j][1]&pow2(2,13))) {//if our only inner cell neighbour is a boundary(obstacle), we set this cell to the same kind of boundary.
				Flag[i][j][0] = getwallbit(0)/*3*(pow(2,10)+pow(2,8)+pow(2,6)+pow(2,4)+4+1)*/ + (Flag[i][j][1]&(pow2(2,12)*15)/*info bout the boundary at the beginning*/);
			} else { //Otherwise set it to what you read in the picture (<- remark: NO. we set it to what we read in the parameter file.). plus subtract/set the neighbouring cell, which seems to be water or air
				Flag[i][j][0] = (getwallbit(wb)-3) + (Flag[i][j][1]&3);
			}
			if ((Flag[i][j][kmax]&pow2(2,12))==(Flag[i][j][kmax]&pow2(2,13))) {//if our only inner cell neighbour is a boundary(obstacle), we set this cell to the same kind of bc.
				Flag[i][j][kmax+1] = getwallbit(0) + (Flag[i][j][kmax]&(pow2(2,12)*15)); /*info bout the boundary at the beginning*/
			} else { //Otherwise set it to what you read in the picture <- remark: NO. we set it to what we read in the parameter file.
				Flag[i][j][kmax+1] = (getwallbit(wt)-12) + (Flag[i][j][kmax]&12);
			}
		}
	}
	//set the outer boundary flags - right and left:
	for (j=1; j<=jmax; j++){
		for (k=1; k<=kmax; k++){
			if ((Flag[1][j][k]&pow2(2,12))==(Flag[1][j][k]&pow2(2,13))) {
				Flag[0][j][k] = getwallbit(0) + (Flag[1][j][k]&(pow2(2,12)*15));
			} else { //Otherwise set it to what you read in the picture <- remark: NO. we set it to what we read in the parameter file.
				Flag[0][j][k] = (getwallbit(wl)- 3*pow2(2,10)) + (Flag[1][j][k]& (3*pow2(2,10)));
			}
			if ((Flag[imax][j][k]&pow2(2,12))==(Flag[imax][j][k]&pow2(2,13))) {
				Flag[imax+1][j][k] = getwallbit(0) + (Flag[imax][j][k]&(pow2(2,12)*15));
			} else {
				Flag[imax+1][j][k] = (getwallbit(wr)-3*pow2(2,8)) + (Flag[imax][j][k]&(3*pow2(2,8)));
			}
		}
	}
	//set the outer boundary flags - front and back:
	for (i=1; i<=imax; i++){
		for (k=1; k<=kmax; k++){
			if ((Flag[i][1][k]&pow2(2,12))==(Flag[i][1][k]&pow2(2,13))) {
				Flag[i][0][k] = getwallbit(0) + (Flag[i][1][k]&(pow2(2,12)*15));
			} else {
				Flag[i][0][k] = (getwallbit(wf)-64*3) + (Flag[i][1][k]&(64*3));
			}
			if ((Flag[i][jmax][k]&pow2(2,12))==(Flag[i][jmax][k]&pow2(2,13))) {
				Flag[i][jmax+1][k] = getwallbit(0) + (Flag[i][jmax][k]&(pow2(2,12)*15));
			} else {
				Flag[i][jmax+1][k] = (getwallbit(wh)-16*3) + (Flag[i][jmax][k]&(16*3));
			}
		}
	}

	if(strcmp (problem,"drop.pgm")==0){
		init_sphere(problem, imax, jmax, kmax, Flag, wl, wr, wf, wh, wt, wb);
	}
	else if(strcmp (problem,"breaking_dam.pgm")==0){
		init_breaking_dam(problem, imax, jmax, kmax, Flag, wl, wr, wf, wh, wt, wb);
	}


	printf("init_cells\n");
	for(i = 1; i < imax+1; i++) {
		for(k = 1; k < kmax+1; k++) {
			for(j = 1; j < jmax+1; j++) {
				init_cell(Flag,i,j,k);

			}
		}
	}
}



/*
	for(i = 0; i <= imax+1; i++) {
		for(j = 0; j <= jmax+1; j++) {
			for(k = 0; k <= kmax+1; k++) {
				if(isfluid(Flag[i][j][k])){
					setcelltype(&Flag[i][jmax][k],FLUID);
				}
			}
		}
	}
 */



// take care of the case when pressure is given
/*	for (i=0; i<=imax+1; i++){
		if (presDelta) {
			Flag[i][0] += 32;
			Flag[i][jmax+1] += 32;
		}
	}
	for (j=0; j<=jmax+1; j++){
		if (presDelta) {
			Flag[0][j] += 32;
			Flag[imax+1][j] += 32;
		}
	}*/





