#include <stdio.h>
#include <stdlib.h>

void chapeau(int* path, int len, int* pathChap){
	for (int i=0; i<len; i++){
		pathChap[i]=(path[len-i-1]+2)%4;
	}
}	

//version quelque peu naive, utilisée car je n'ai pas réussi l'implémentation de l'autre méthode et que la différence est minime  
int naive(int* path, int* pathC, int len){
	int ps;
	int i4;
	for (int i1=0; i1<len/2; i1++){
		for (int i2=i1+1; i2<i1-1+len/2; i2++){
			for (int i3=i2+1; i3<=i1+len/2; i3++){
				i4=len/2+i1;
				ps=1;			
			//	printf("%d %d %d %d\n", i1, i2, i3, i4);
				for (int x=i1; x<i2; x++){
					ps*=(path[x]==pathC[((3*len/2-i2)%len+len+x-i1)%len]);
			//	       printf("%d %d\n",x,((3*len/2-i2)%len+len+x-i1)%len);
				}
				for (int x=i2; x<i3; x++){
					ps*=(path[x]==pathC[((3*len/2-i3)%len+len+x-i2)%len]);
			//	       printf("%d %d\n",x,((3*len/2-i3)%len+len+x-i2)%len);
				}
				for (int x=i3; x<i4; x++){
					ps*=(path[x]==pathC[((3*len/2-i4)%len+len+x-i3)%len]);
			//	       printf("%d %d\n",x,((3*len/2-i4)%len+len+x-i3)%len);
				}
				if (ps)
					return 1;
			}
		}
	}
	return 0;
}


//version plus recherchée, qui ne fonctionne pas actuellement.
int admissible(int pos, int* path, int* pathC, int len){
	int x1,x2,y1,y2;
	int posbis=-1;
	
	int* admissA=(int*) malloc(2*len*sizeof(int));
	admissA[0]=0;

	for (int i=0; i<len; i++){
		x1=pos;
		y1=(pos+1)%len;
		if (pathC[i]==path[pos]){
			x2=i;
			y2=(i+1)%len;
			int j=1;
			while (pathC[(i+j)%len]==path[(pos+j)%len])
			{
				y1++;
				y2++;
				j++;
			}
			j=1;
			while (pathC[(i-j+len)%len]==path[(pos-j+len)%len])
			{
				x1--;
				x2--;
				j++;
			}

			int lcomp=len-2*(y1-x1);
			y1=(len+y1)%len;
			y2=(len+y2)%len;
			if (x1<0)
				x1+=len;
			if (x2<0)
				x2+=len;
			if((2*len-y2-y1)%len==((len+x1)-(len-x2))%len)
			{	
				int ps_sq=1;
				for (int k=0; k<lcomp/2; k++){
					if (path[(k+y1)%len] != (path[(len+x1-1-k)%len]+2)%4)	
						ps_sq=0;
				}
				if (ps_sq==1)
					return 1;
			}

			admissA[2*admissA[0]+1]=x1;
			admissA[2*admissA[0]+2]=y1;
			admissA[0]++;
			if (y1>posbis)
				posbis=y1;
		}	
	}


	for (int i=0; i<len; i++){
		x1=posbis;
		y1=posbis+1;
		if (pathC[i]==path[posbis]){
			x2=i;
			y2=i+1;
			int j=1;
			while (pathC[(i+j)%len]==path[(posbis+j)%len])
			{
				y1++;
				y2++;
				j++;
			}
			j=1;
			while (pathC[(i-j+len)%len]==path[(posbis-j+len)%len])
			{
				x1--;
				x2--;
				j++;
			}

			y1=(len+y1)%len;
			y2=(len+y2)%len;
			x1=(len+x1)%len;
			x2=(len+x2)%len;

			for (int l=0; l<admissA[0]; l++){
				if (admissA[2*l+2]==x1)
				{
					int lcomp=len-2*((len+y1-admissA[2*l+1])%len);
					int ps_hex=1;
					for (int k=0; k<lcomp/2; k++){
						if (path[(k+y1)%len]!=(path[(len+admissA[2*l+1]-k)%len]+2)%4)	
							ps_hex=0;
					}
					if (ps_hex==1)
					{
					//	printf("pseudo_hex!\n");
						return 1;
					}
				}
			}

		}
	}
	return 0;	
}

int bncrit(int* path, int len){
	int* pathC =(int*)  calloc(len, sizeof(int));
	chapeau(path, len, pathC);
	//int ps=admissible(0, path, pathC, len);
	int psb=bourrin(path, pathC, len);
	free(pathC);

	return psb;
}
