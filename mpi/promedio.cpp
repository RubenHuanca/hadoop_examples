/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <ctime>
//#include "btree.h"

using namespace std;

string wordMean(long ini, long fin, const char  *arch, int pr)
{
	FILE *file = fopen(arch,"r");
	char c;
	fseek(file, ini, SEEK_SET);
	if(ini != 0)
		while((c = fgetc(file)) != ' ') ++ini;

	fseek(file, fin, SEEK_SET);
	while((c = fgetc(file)) != EOF && c != ' ') ++fin;

	fseek(file, ini, SEEK_SET); 
	char word[100];

  long cont = 0, sum = 0;
	long pos;
	while(fin > ftell(file)){
		pos = fscanf(file,"%s",word);
		sum += strlen(word);
    ++cont;
	if(cont %4000000 == 0)
		printf("Palabras: %ld del procesador: %d\n", cont,pr);

	}
	//printf("Palabras:%ld Caracteres:%ld\n", cont, sum);

	fclose(file);

  char res[40];
  sprintf(res,"%ld %ld",cont,sum);

	return res;
}
 

int main (int argc, char* argv[])
{
  time_t time_ini, time_fin;
  double diferencia = 0;
  int minutos;
  int segundos = 0;
  int temp;
  int rank, size;

  FILE *file = fopen("book1.txt","r");
  fseek(file, 0, SEEK_END);
  long tam = ftell(file);
  fclose(file);

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);       
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  char res[40];
  float interval = 1.0*tam/(size-1);          
  if(rank != 0)
  { 
  	long ini = (rank-1)*interval;
  	long fin = (rank)*interval;
  	strcpy(res,wordMean(ini, fin, "book1.txt",rank).c_str());

  	MPI_Send(res, strlen(res)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }
  else{
    time_ini = time(NULL); 
  	FILE *es = fopen("promedio100.txt","w");
    long cont = 0,sum = 0, tmpC, tmpS;
  	for(int pr = 1;pr < size; ++pr)
  	{
  		MPI_Recv(res, 40, MPI_CHAR, pr, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      sscanf(res,"%ld %ld",&tmpC,&tmpS);
      cont += tmpC;
      sum += tmpS;
  	}

    double promedio = 1.0 * sum / cont;

    fprintf(es,"Caracteres -> %ld\nPalabras -> %ld\nPromedio -> %f\n", sum, cont, promedio);
    printf("Caracteres -> %ld\nPalabras -> %ld\nPromedio -> %f\n", sum, cont, promedio);

    time_fin = time(NULL); 
    diferencia = difftime(time_fin, time_ini);
      minutos = diferencia / 60.0;
      segundos = diferencia - minutos * 60;

      if (temp != segundos){ /* ha transcurrido un segundo */
         temp = segundos; /* guarda el valor previo */
         char bufer[100];
         sprintf (bufer,"%02d:%02d", minutos, segundos);
         fprintf(es,"Tiempo -> %s\n", bufer);
         cout<<"Tiempo: "<<bufer<<endl;
      }


  	fclose(es);


  }

  MPI_Finalize();
  
  return 0;
}
