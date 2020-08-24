#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gurobi_c.h"



typedef struct s_List List;
struct s_List
{
    List *next; 
    int mat; 
    int occ;
};

List * create(int motif, List* n){
	List *list = (List*) malloc(sizeof(List));           
	if (list)
	{
		list -> mat = motif;
		list -> next = n;
		list -> occ =1;
	}
	return list;
}

void freel(List* list){
	if (list==NULL)
		return;
	freel(list->next);
	free(list);
}

int getindex(int x, int* index){
	int lim=index[0];
	for (int i=1; i<lim; i++){
	       	if (x<index[i])
		{
			return i-1;
		}
	}
	printf("ERROR\n");
	return -1;
}

void printmotif(int motif, int occ, int w){
	printf("%d  (x%d)\n", motif, occ);
}

void printlist(List* l, int w){
	if (l!=NULL){
		printmotif(l->mat, l->occ, w);
		printlist(l->next, w);
	}	
}

void addocc(List* l){
	l -> occ +=1;
}	


typedef struct eq_List EqL;
struct eq_List
{
    EqL *next; 
    int *coeff; 
};

EqL * createEqL(int* equat, EqL* n){
	EqL *elist = (EqL*) malloc(sizeof(EqL));  
	if (elist)
	{
		elist -> coeff = equat;
		elist -> next = n;
	}
	return elist;
}

void freeleq(EqL* list){
	if (list==NULL)
		return;
	freeleq(list->next);
	free(list->coeff);
	free(list);
}
unsigned int Xrotate(unsigned int hull, int w){
        unsigned int bis=0;
        for (int i=0; i<w/2; i++){
                for (int j=0; j<(w+1)/2; j++){
                        bis|=(1<<(i*w+j))*((hull>>(j*w+(w-i-1)))&1);
                        bis|=(1<<(j*w+(w-i-1)))*((hull>>((w-i-1)*w+(w-j-1)))&1);
                        bis|=(1<<((w-i-1)*w+(w-j-1)))*((hull>>((w-j-1)*w+i))&1);
                        bis|=(1<<((w-j-1)*w+i))*((hull>>(i*w+j))&1);
                }
        }
	bis |= (1<<((w*w)/2))*((hull>>(((w*w)/2)))&1);
	return bis;
}

unsigned int XsymH(unsigned int hull, int w){
	unsigned int bis=0;
        for (int j=0; j<w; j++){
                for (int i=0; i<w; i++){
                        bis|=(1<<(i*w+j))*((hull>>((w-i-1)*w+j))&1);
                }
        }
	return bis;
}

unsigned int XsymV(unsigned int hull, int w){
	unsigned int bis=0;
        for (int i=0; i<w; i++){
                for (int j=0; j<w; j++){
                        bis|=(1<<(i*w+j))*((hull>>((i+1)*w-j-1))&1);
                }
        }
	return bis;
}

unsigned int canon(unsigned int motif, int w, int debug){
	unsigned int ref=motif;
	for (int i=0; i<7; i++){
		if (i==3) 
		{
			motif=Xrotate(motif, w);
			if (motif<ref)
			{
				ref=motif;
			}

		}		
		else if (i%2==1) 
		{
			motif=XsymH(motif, w);
			if (motif<ref)
			{
				ref=motif;
			}
			
		}
		else 
		{
			motif=XsymV(motif, w);
			if (motif<ref)
			{
				ref=motif;
			}
			
		}
	}
	return(ref);
}

int search(int motif, List* list, int D){
	if (list != NULL){
		if (motif==(list->mat))
		{
			addocc(list);
			return 1;
		}
		else
		{
			return (search(motif, list->next, D));
		}		
	}
	return 0;
}



int llength(List* l){
	if (l==NULL){
		return 0;
	}
	return (1+llength(l->next));
}


void genList(int radius, int N, int* coord, int w, int h, List** l1, List** l2){
	int D = 2*radius +1;
	int inl1, empt;
	for (int i=0; i<h+2*radius; i++){
		for (int j=0; j<w+2*radius; j++){
			unsigned int motif=0;
			inl1=0;
			for (int k=0; k<N; k++){
				int x=coord[2*k];
				int y=coord[2*k+1];
				x=x-j+2*radius;
				y=y-i+2*radius;
				if (0<=x && x<D && 0<=y && y<D)
				{
					motif|=(1<<(y*D+x));
				}
				if (x==radius && y==radius)
					inl1=1;
			}
			motif = canon(motif, D, inl1);
			if (inl1)
			{
				if (search(motif, *l1, D)==0)
				{
					*l1 = create(motif, *l1);
				}	
			}
			else
			{
				if (motif!=0 && search(motif, *l2, D)==0)
				{	
					*l2 = create(motif, *l2);
				}
			}		
		}
	}		
}

int addmat(int* confpart, int motif, int t, int ind){
	if ((*confpart&motif)==0){
		*confpart|=motif;
		return 1;
	}
	return 0;
}

void concatEqL(EqL** A, EqL* B){
	if ((*A)==NULL)
	{
		*A=B;
	}
	else if ((*A)->next == NULL)
	{
		(*A)->next = B;
	}
	else
	{
		concatEqL(&((*A)->next), B);
	}
}

											
EqL* addmot(int confpart, List* l2, int D, int ind, int d2, int* coeffs, int x, int* index, int c1){
	int b=0;
	if (confpart==((1<<(D*D))-1))
	{	
		int* eq=(int*) malloc((2*(x+1)+1)*sizeof(int));   
		eq[0]=x+1;
		for (int j=0; j<2*(x+1); j++){
			eq[j+1]=coeffs[j];
		}
		EqL* el = createEqL(eq, NULL);
		return el;
	}
	if (l2==NULL){
		return NULL;
	}
	EqL* elist = addmot(confpart, l2->next, D, ind, d2+1, coeffs, x, index, c1);
	if (addmat(&confpart, l2->mat, D*D, ind))
	{
		if (coeffs[2*x]==c1+getindex(d2, index))
		{
			b=1;
			coeffs[2*x+1]+=1;
		}
		else
		{
			b=0;
			x+=1;
			coeffs[2*x]=c1+getindex(d2, index);
			coeffs[2*x+1]=1;
		}
		EqL* el = addmot(confpart, l2->next, D, ind+1, d2+1, coeffs, x, index, c1);
		concatEqL(&elist, el);
		coeffs[2*x+1]-=b;
	}
	
	return elist;
}

EqL * genEq(int radius, List* l1, List* l2, int d1, int* index, int c1){
	if (l1==NULL){
		return NULL;
	}
	int D=2*radius+1;
	EqL* elist = genEq(radius, l1->next, l2, d1+1, index, c1);
	
	int* coeffs=(int*) calloc(2*(D*D), sizeof(int));    
			//pour plus tard: ajouter une case pour différencier les orientations ?
	coeffs[0]=d1;
	coeffs[1]=1;
	EqL* el = addmot(l1->mat, l2, D, 2, c1, coeffs, 0, index, c1);
	if (el == NULL)
	{
		return elist;
	}
	concatEqL(&el, elist);	
	free(coeffs);
	return el;
}

int checkEq(EqL* el, int de, int* ind, double* val, int j, int d, int D){
	if(el==NULL){
		return j;
	}
	int i=0;
	while (i<(el->coeff[0])){  
		if ((el->coeff)[2*i+1]==de && !(i>0 && de==0))
		{
			ind[j]=d;
			val[j]=(el->coeff)[2*(i+1)];
			j+=1;
		}
		i+=1;
	}
	return checkEq(el->next, de, ind, val, j, d+1, D);
}

int genCons(int error, List* l1, List* l2, int c1, EqL* elist, GRBenv *env, GRBmodel *model, int* ind, double* val, int i, int D){
	if (l1==NULL)
	{	
//		printf("contraintes de type 2\n");
		if (l2==NULL || elist==NULL)
		{
			if (l2==NULL) 
			return error;
		}
		int j=checkEq(elist, i+1, ind, val, 0, 0, D);
		error = GRBaddconstr(model, j, ind, val, GRB_EQUAL, l2->occ, NULL);
		return genCons(error, l1, l2->next, c1, elist, env, model, ind, val, i+1, D);
	}
	int j=checkEq(elist, i, ind, val, 0, 0, D);		
	error = GRBaddconstr(model, j, ind, val, GRB_EQUAL, l1->occ, NULL);
	//TODO exploiter l'erreur (là on la recouvre à chaque fois..)
	return genCons(error, l1->next, l2, c1, elist, env, model, ind, val, i+1, D);	

}



int length(EqL* el){
	if (el==NULL){
		return 0;
	}
	return (1+length(el->next));
}

List* expand(List* l, int D, int* index, int d){
	if (l==NULL){
		return NULL;
	}
	List* l2nextbis = expand(l->next, D, index, d+1);
	
	unsigned int a0;
	unsigned int a1;
	unsigned int a2;
	unsigned int a3;
	unsigned int a4;
	unsigned int a5;
	unsigned int a6;
	unsigned int a7;
	
	a0=l->mat;
	a1=XsymH(a0, D);
	a2=XsymV(a1, D);
	a3=XsymH(a2, D);
	a4=Xrotate(a3, D);
	a5=XsymH(a4, D);
	a6=XsymV(a5, D);
	a7=XsymH(a6, D);

	int x=1;

	List* l7=create(a7, l2nextbis);
	List* nl=l7;
	if (a6!=a7)
	{
		List* l6=create(a6, nl);
		nl=l6;
		x++;
	}

	if (a5!=a7 && a5!=a6)
	{
		List* l5=create(a5, nl);
		nl=l5;
		x++;
	}

	if (a4!=a7 && a4!=a6 && a4!=a5)
	{
		List* l4=create(a4, nl);
		nl=l4;
		x++;
	}
	if (a3!=a7 && a3!=a6 && a3!=a5 && a3!=a4)
	{
		List* l3=create(a3, nl);
		nl=l3;
		x++;
	}
	if (a2!=a7 && a2!=a6 && a2!=a5 && a2!=a4 && a2!=a3)
	{
		List* l2=create(a2, nl);
		nl=l2;
		x++;
	}
	if (a1!=a7 && a1!=a6 && a1!=a5 && a1!=a4 && a1!=a3 && a1!=a2)
	{
		List* l1=create(a1, nl);
		nl=l1;
		x++;
	}
	if (a0!=a7 && a0!=a6 && a0!=a5 && a0!=a4 && a0!=a3 && a0!=a2 && a0!=a1)
	{
		List* l0=create(a0, nl);
		nl=l0;
		x++;
	}
	for (int i=d+2; i<index[0]; i++){
		index[i]+=x;
	}
	return (nl);
}





int guroLP(int c1, int c2, EqL* elist, List* l1, List* l2, int N, int D){
  	int error = 0;
	int x=0;	
	int nbv=length(elist);

	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;

	double* sol=calloc(nbv, sizeof(double));   
	int* ind=calloc(nbv, sizeof(int));  
	double* val=calloc(nbv, sizeof(double));  
	double* obj=calloc(nbv, sizeof(double));   
	char* vtype=calloc(nbv, sizeof(char)); 
	int optimstatus;
	double objval;

	/* Create environment */
	error = GRBemptyenv(&env);
	if (error) goto QUIT;

	error = GRBsetstrparam(env, "LogFile", "/dev/null"); //redirige log vers null
	if (error) goto QUIT;

	error = GRBstartenv(env);
	if (error) goto QUIT;

	/* Create an empty model */
	error = GRBnewmodel(env, &model, "genLP", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) goto QUIT;

	/* Add variables */
	for (int i=0; i<nbv; i++){
		obj[i]=1;
		vtype[i]=GRB_CONTINUOUS;
	}

	double *lb=calloc(nbv, sizeof(double));   
	double *ub=malloc(nbv*sizeof(double));  
	for (int i=0; i<nbv; i++){
		ub[i]=N;
	}
	
	error = GRBaddvars(model, nbv, 0, NULL, NULL, NULL,  obj, lb, ub, vtype, NULL);
	if (error) goto QUIT;

	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) goto QUIT;
	
	error = genCons(0, l1, l2, c1, elist, env, model, ind, val, 0, D);
	if (error) goto QUIT;
	
	/* Optimize model */
	error = GRBoptimize(model);
	if (error) goto QUIT;

	/* Write model to 'mip1.lp' */
/*	error = GRBwrite(model, "genLP.lp");
	if (error) goto QUIT;
*/
	/* Capture solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) goto QUIT;

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) goto QUIT;

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, nbv, sol);
	if (error) goto QUIT;


//	printf("\nOptimization complete\n");
	if (optimstatus == GRB_OPTIMAL) {
		x=1;
/*	printf("Optimal objective: %.4e\n", objval);
	for (int i=0; i<nbv; i++){
		printf("d%d = %.3f, ", i, sol[i]);
*/	}
/*	printf("\n\n");
	} else if (optimstatus == GRB_INF_OR_UNBD) {
//	printf("Model is infeasible or unbounded\n");
	} else {
//	printf("Optimization was stopped early\n");
	}
*/
QUIT:

	/* Error reporting */
	if (error) {
	printf("ERROR: %s\n", GRBgeterrormsg(env));
	}

	/* Free model */
	GRBfreemodel(model);

	/* Free environment */
	GRBfreeenv(env);

	free(sol);
	free(ind);
	free(val);
	free(obj);
	free(vtype);
	free(lb);
	free(ub);
	freel(l1);
	freel(l2);
	freeleq(elist);
	return x;
}

int genLP(int* coord, int N, int height, int width, int radius)
{
	//generation listes motifs
	List *l1 = NULL; //create(0, NULL);
	List *l2 = NULL; //create(0, NULL);
	genList(radius, N, coord, width, height, &l1, &l2);
	
	if (l1==NULL)
	{
		printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	}
	int c1=llength(l1);
	int c2=llength(l2);
	
/*	printf("Listes des motifs calculées: \nl1 de taille: %d \n", c1);
	printlist(l1, radius*2+1);
	printf("l2 de taille: %d \n", c2);
	printlist(l2, radius*2+1);
*/	
	int* index=calloc(c2+2,sizeof(int)); 
	for (int i=1; i<c2+2; i++){
		index[i]=c1;
	}
	index[0]=c2+2;
	List* l2bis = expand(l2, radius*2+1, index, 0);

/*	printf("l2 expanded %d\n", llength(l2bis));
	printlist(l2bis, radius*2+1);
*/
	c1=llength(l1);
	//generation equations
	EqL* elist = genEq(radius, l1, l2bis, 0, index, c1);
	if (elist==NULL)
	{
		printf("Pas d'équations...\n\n\n\n");
	}
	freel(l2bis);	
	free(index);
	return guroLP(c1, c2, elist, l1, l2, N, 2*radius+1);	
}




int mainLP(int N, int* coord, int radius, int width, int height)
{
	return genLP(coord, N, height, width, radius);
	
}
