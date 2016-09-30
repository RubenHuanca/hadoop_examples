/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <time.h>
//#include "btree.h"

using namespace std;



unordered_map<string, int> *wordCount(long ini, long fin, const char  *arch, int pr)
{
	FILE *file = fopen(arch,"r");
	char c;
	fseek(file, ini, SEEK_SET);
	if(ini != 0)
		while((c = fgetc(file)) != ' ') ++ini;

	fseek(file, fin, SEEK_SET);
	while((c = fgetc(file)) != EOF && c != ' ') ++fin;

	fseek(file, ini, SEEK_SET);
	unordered_map<string, int> *myHash = new unordered_map<string, int>; 
	char word[100];
	long cont = 0;
	int pos;

	while(fin > ftell(file)){
		++cont;
		pos = fscanf(file,"%s",word);
		++(*myHash)[word];
		if(cont % 4000000==0)
		  printf("Palabras: %ld  del proceso: %d \n",cont, pr);
	}

	fclose(file);

	return myHash;
}
 

int main (int argc, char* argv[])
{

	time_t time_ini, time_fin;
	double diferencia = 0;
	int minutos;
	int segundos = 0;
	int temp;

  int rank, size;

  FILE *file = fopen("book.txt","r");
  fseek(file, 0, SEEK_END);
  long tam = ftell(file);
  fclose(file);

  char registro[8000000];
  unordered_map<string, int> *myHash;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);       
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  float interval = 1.0*tam/(size-1);          
  if(rank != 0)
  {
  	char word[200];  	
  	long ini = (rank-1)*interval;
  	long fin = (rank)*interval;
  	myHash = wordCount(ini, fin, "book.txt",rank);

  	string buf="";
	
	//printf("Llegue!\n");
  	for(auto& x: *myHash)
    {
  		sprintf(word," %s %d",x.first.c_str(),x.second);
  		buf += word;
  		//strcat(registro, word); //Falla cuando se le llama mucho > 500k
  	}

  	strcpy(registro,buf.c_str());
	//printf("Palabras %ld proceso: %d\n",(*myHash).size(),rank);

  	delete myHash;
  	myHash = 0;
  	MPI_Send(registro, strlen(registro) +1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }
  else{

	time_ini = time(NULL); 

  	myHash = new unordered_map<string, int>;
  	FILE *es = fopen("contador1.txt","w");
  	int fre;
  	for(int pr = 1;pr < size; ++pr)
  	{
  		MPI_Recv(registro,8000000, MPI_CHAR, pr, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		char *pal = strtok (registro," "), *num;
  		while (pal != NULL)
   	  {
		  	num = strtok (NULL, " ");
		  	sscanf(num,"%d",&fre);
		    (*myHash)[pal]+=fre;
		    pal = strtok (NULL, " ");
		  }
  	}


  	time_fin = time(NULL); 
  	diferencia = difftime(time_fin, time_ini);
      minutos = diferencia / 60.0;
      segundos = diferencia - minutos * 60;

      if (temp != segundos){ /* ha transcurrido un segundo */
         temp = segundos; /* guarda el valor previo */
         char bufer[100];
         sprintf (bufer,"%02d:%02d", minutos, segundos);
         cout<<"Tiempo: "<<bufer<<endl;
      }



    fprintf(es,"Hay %d palabras\n",(int)(*myHash).size());
    printf("Hay %d palabras\n",(int)(*myHash).size());

  	for(auto& x: *myHash)
  	{
  		//printf("%s %d\n",x.first.c_str(),x.second);
  		fprintf(es,"%s %d\n",x.first.c_str(),x.second);
  	}

  	fclose(es);
  	delete myHash;
  	myHash = 0;

  }

  
  MPI_Finalize();

  
  
  return 0;
}
