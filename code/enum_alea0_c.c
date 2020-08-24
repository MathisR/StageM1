#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "genLPbit_c.c"
#include "beauquier_nivat.c"
/*#include "backt.cpp"*/
#include <sys/resource.h> 
#define N 11
#define R 2
#define C0 25
#define C1 1000

#define WIDTH (2*N-2)

//code invalide si N<2 !

/*
void debug(int mat[]){
	printf("\n");
	for (int i=0; i<(N)*(2*N-2); i++){
		if (i%(2*N-2)==0){
			printf("\n");
		}
		char* c=" ";
		if (mat[i]<0 || mat[i]>9){
			c="";
		}
		printf("%s%d ",c, mat[i]);
	}
	printf("\n");


}
*/

int* init(){
	int* mat=malloc(N*WIDTH*sizeof(int));
	for (int i=0; i<N; i++){
		for (int j=0; j<WIDTH; j++){
			if (i+j>WIDTH || j<i-1 || (i==0 && j<(N-2)))
			{
				mat[i*WIDTH+j]=-1; 
			}
			else
			{
				mat[i*WIDTH+j]=0;
			}		
		}
	}
	mat[N-2]=1;
	return mat;
}


void update(int x, int y, int* n, int mat[], int avail[]){
	int coord=y*WIDTH+x;
	if (x && mat[coord-1]==0)
	{
		avail[2*(*n)]=x-1;
		avail[2*(*n)+1]=y;
		*n+=1;
		mat[coord-1]=*n;
	}
	if (y<N-1 && mat[coord+WIDTH]==0)
	{
		avail[2*(*n)]=x;
		avail[2*(*n)+1]=y+1;
		*n+=1;
		mat[coord+WIDTH]=*n;
	}
	if (x<WIDTH-1 && mat[coord+1]==0)
	{
		avail[2*(*n)]=x+1;
		avail[2*(*n)+1]=y;
		*n+=1;
		mat[coord+1]=*n;
	}
	if (y && mat[coord-WIDTH]==0)
	{
		avail[2*(*n)]=x;
		avail[2*(*n)+1]=y-1;
		*n+=1;
		mat[coord-WIDTH]=*n;
	}
}

void downdate(int x, int y, int nsave, int mat[]){
	int coord=y*WIDTH+x;
	if (x && mat[coord-1]>nsave)
	{
		mat[coord-1]=0;
	}
	if (y<N-1 && mat[coord+WIDTH]>nsave)
	{
		mat[coord+WIDTH]=0;
	}
	if (x<WIDTH-1 && mat[coord+1]>nsave)
	{
		mat[coord+1]=0;
	}
	if (y && mat[coord-WIDTH]>nsave)
	{
		mat[coord-WIDTH]=0;
	}

}

void display(int chosen[]){
	int* disp=calloc(N*(WIDTH),sizeof(int));
	for (int i=0; i<N; i++){
		disp[chosen[2*i]+(WIDTH)*chosen[2*i+1]]=1;
	}
	printf("\n");
	for (int i=0; i<(N)*(WIDTH); i++){
		if (i%(2*N-2)==0){
			printf("\n");
		}
		if (disp[i]==1){
			printf("O");
		}
		else{
			printf(" ");
		}
	}
	printf("\n");
	free(disp);

}


unsigned long long shaperef(int hull[], int hullsize){
	unsigned long long res=0;
	unsigned long long pow=1;
       	for (int i=0; i<hullsize; i++){
		res+=pow*hull[i];
		pow*=2;
	}
	return res;
}

void rotate(int hull[], int w){	
	int c;
	for (int i=0; i<w/2; i++){
		for (int j=0; j<(w+1)/2; j++){
			c=hull[i*w+j];
			hull[i*w+j]=hull[j*w+(w-i-1)];
			hull[j*w+(w-i-1)]=hull[(w-i-1)*w+(w-j-1)];
			hull[(w-i-1)*w+(w-j-1)]=hull[(w-j-1)*w+i];
			hull[(w-j-1)*w+i]=c;
		}
	}
}

void symH(int hull[], int h, int w){
	int c;
	for (int j=0; j<w; j++){
		for (int i=0; i<h/2; i++){
			c=hull[i*w+j];
			hull[i*w+j]=hull[(h-i-1)*w+j];
			hull[(h-i-1)*w+j]=c;
		}
	}

}

void symV(int hull[], int h, int w){
	int c;
	for (int i=0; i<h; i++){
		for (int j=0; j<w/2; j++){
			c=hull[i*w+j];
			hull[i*w+j]=hull[(i+1)*w-j-1];
			hull[(i+1)*w-j-1]=c;
		}
	}
}

int dfs(int* area, int x, int y, int w, int h){
	int c=0;
	if (area[y*w+x]==0){
		c=1;
		area[y*w+x]=2;
		if (x+1<w){
			c+=dfs(area, x+1, y, w, h);
		}
		if (x>0){
			c+=dfs(area, x-1, y, w, h);
		}
		if (y>0){
			c+=dfs(area, x, y-1, w, h);
		}
		if (y+1<h){
			c+=dfs(area, x, y+1, w, h);
		}
	}
	return c;
}	

int nextpath(int objx, int objy, int* path, int z, int* area, int w, int* x, int* y, int* dx, int* dy){
	int swap;
	if (*x==objx && *y==objy)
	{	
		return z;
	}
	if (area[*x-(*dx==-1 || *dy==-1)+w*(*y-(*dy==-1 || *dx==1))]==1)
	{

		swap=-*dx;
		*dx=*dy;
		*dy=swap;
		*x=*x+*dx;
		*y=*y+*dy;
		path[z]=(1-*dx)+2*(*dy==-1);
		return nextpath(objx, objy, path, z+1, area, w, x, y, dx, dy); 
	}
	if (area[*x-(*dx==-1 || *dy==1)+w*(*y-(*dx==-1 || *dy==-1))]==1)
	{
		*x=*x+*dx;
		*y=*y+*dy;
		path[z]=(1-*dx)+2*(*dy==-1);
		return nextpath(objx, objy, path, z+1, area, w, x, y, dx, dy); 
	}
	swap=*dx;
	*dx=-*dy;
	*dy=swap;
	*x=*x+*dx;
	*y=*y+*dy;
	path[z]=(1-*dx)+2*(*dy==-1);
	return nextpath(objx, objy, path, z+1, area, w, x, y, dx, dy); 
}

void computepath(int* path, int* area, int* len, int w){
	int x=1;
	int y=1;
	int dx=1;
	int dy=0;
	while (area[y*w+x]!=1){
		x+=1;
	}
	path[0]=0;
	x+=1;
	*len = nextpath(x-1, y, path, 1, area, w, &x, &y, &dx, &dy);
}

int hole(int* area, int chosen[], int amplx, int amply, int minx){

	for (int i=0; i<N; i++){
		area[(chosen[2*i+1]+1)*(amplx)+(chosen[2*i]-minx+1)]=1;
	}
	int empt = dfs(area, 0, 0, amplx, amply);
	if (empt+N==(amplx*amply)){
		return 0;
	}
	return 1;
	
}


void hull(int chosen[], long long* cfree, int* notpav, int* psqh){
	int minx=N-2;
        int maxx=N-2;
        int maxy=0;
	int centroidx=0;
	int centroidy=0;
	for (int i=0; i<N; i++){
		minx = (chosen[2*i]<minx) ? chosen[2*i] : minx;
		maxx = (chosen[2*i]>maxx) ? chosen[2*i] : maxx;
		maxy = (chosen[2*i+1]>maxy) ? chosen[2*i+1] : maxy;
		centroidx += chosen[2*i];
		centroidy += chosen[2*i+1];
	}
	centroidx -= N*minx;
	int amplx=maxx-minx; 
	int* area=calloc((amplx+3)*(maxy+3), sizeof(int));	
	if (hole(area, chosen, amplx+3, maxy+3, minx)){
		free(area);
		return;
	}
	int aA=*cfree;
	if (((maxy==amplx) && (centroidx*2 <= amplx*N) && (centroidy*2 <= maxy*N) && (centroidy>=centroidx))
				|| ((maxy>amplx) && (centroidx*2 <= amplx*N) && (centroidy*2 <= maxy*N)))
	{
		int nh, nv, nr;
		nr = maxy==amplx && centroidy==centroidx; 
		nh = centroidy*2==maxy*N;
		nv = (maxy==amplx && centroidy==centroidx) || (maxy>amplx && centroidx*2==amplx*N);
		
		if (!(nr || nv || nh)){
			*cfree+=1;
			amplx++;
			maxy++;
		}
		else{
			amplx++;
			maxy++;
			int *hull=calloc(amplx*maxy,sizeof(int));	
			for (int i=0; i<N; i++){
				hull[chosen[2*i+1]*amplx+(chosen[2*i]-minx)]=1;
			}
			unsigned long long ref=shaperef(hull, amplx*maxy);		
			int fre=1;
			if (!(nr || nv))
			{
				symH(hull, maxy, amplx);
				fre *= (ref <= shaperef(hull, amplx*maxy));
			}
			else if (!nh)
			{
				symV(hull, maxy, amplx);
				if (nr)
				{
					rotate(hull, amplx);
				}
				fre *= (ref <= shaperef(hull, amplx*maxy));
			}
			else {
				for (int i=0; i<7-4*(1-nr); i++){
					if (i==3) 
					{
						rotate(hull, amplx);
						fre *= (ref <= shaperef(hull, amplx*maxy));
					}		
					else if (i%2==1) 
					{
						symH(hull, maxy, amplx);
						fre *= (ref <= shaperef(hull, amplx*maxy));
					}
					else 
					{
						symV(hull, maxy, amplx);
						fre *= (!nv || ref <= shaperef(hull, amplx*maxy));
					}
				}
			}
			*cfree+=fre;
			free(hull);
		}
	}
	if (*cfree!=aA)
	{
		int* path=malloc((N+1)*2*sizeof(int));
		int lenpath=0;
		computepath(path, area, &lenpath, amplx+2);
	//TODO à enlever quand bug résolu
	//
	//
/*	
		int x=bncrit(path, lenpath);
		if (x==1){
			printf("\naccepté, refusé par bourrin\n");
			display(chosen);
		}
		else if (x==2){
			printf("\nrefusé, accepté par bourrin\n");
			display(chosen);
		}
		free(path);
		free(area);
		return;
*/	//
	//
	//TODO jusque là

		if (bncrit(path, lenpath))
		{
			*psqh+=1;
			free(area);
			free(path);
			return;
		}
		free(path);
		int* coord=calloc(2*N, sizeof(int));
		for (int i=0; i<N; i++){
			coord[2*i]=chosen[2*i]-minx;
			coord[2*i+1]=chosen[2*i+1];
		}
		//*notpav += 1-mainBT(N, coord);


	 	*notpav += 1-mainLP(N, coord, R, amplx, maxy); 
		free(coord);
	}
	free(area);
}

void explore(int k, int n, int mat[], int avail[], int last, long long* c, long long* cfree, int* notpav, int chosen[], int* psqh, long long* selec){
	if (k<N){
		int x,y;
		int nsave = n;
		for (int l=last+1; l<=nsave-1; l++)
		{
			n=nsave;	
			x=avail[2*l];
			y=avail[2*l+1];
			chosen[2*k]=x;
			chosen[2*k+1]=y;
			update(x, y, &n, mat, avail);
			explore(k+1, n, mat, avail, l, c, cfree, notpav, chosen, psqh, selec);
			downdate(x, y, nsave, mat);
		}
	}
	else{
		*c+=1;	
		int x=random()%C1;
		if (x<C0){
			hull(chosen, cfree, notpav, psqh);
			*selec += 1;
		}
	}
}

int main(int argc, char *argv[])
{
	srandom(time(NULL));
       const rlim_t kStackSize = 1024L * 1024L * 1024L;   // min stack size = 64 Mb
       struct rlimit rl;
       int result;

       result = getrlimit(RLIMIT_STACK, &rl);
       if (result == 0)
       {
           if (rl.rlim_cur < kStackSize)
           {
               rl.rlim_cur = kStackSize;
               result = setrlimit(RLIMIT_STACK, &rl);
               if (result != 0)
               {
                   fprintf(stderr, "setrlimit returned result = %d\n", result);
               }
           }
       }
        int *mat=init();
	int n=1;
	int last=-1;
	int *avail=malloc(N*8*sizeof(int));
	if (mat==NULL || avail==NULL){
		printf("Erreur allocation mémoire initiale\n");
		exit(0);
	}
	avail[0]=N-2;
	avail[1]=0;
	int *chosen=malloc(N*2*sizeof(int));
	
	long long count=0;
	long long selec=0;
	long long countfree=0;
	int notpav=0;
	int psqh=0;

	explore(0, n, mat, avail, last, &count, &countfree, &notpav, chosen, &psqh, &selec);
	
	free(avail);
	free(mat);
	free(chosen);

	FILE* sortie = fopen("zChance0.out", "a");	
	fprintf(sortie, "\nN=%d, R=%d, C0=%d/C1=%ld \nTotal:\n %lld Fixed Form Polyominoes, %lld selected (pseudo)randomly \n %lld Free Form Polyominoes\n %d Pseudo-Squares or Pseudo-Hexagons (Beauquier-Nivat Characterisation)\n %d FreeFP NOT paving the plane (for sure)\n\n Unsure for %lld polyominoes even if they passed the test with R=%d", N, R, C0, C1, count, selec, countfree, psqh, notpav, countfree-psqh-notpav, R);
	fclose(sortie);
	return 0;
}
 
