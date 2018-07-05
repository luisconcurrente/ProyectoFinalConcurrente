	/*
 ============================================================================
 Name        : Prueba1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C 
 ============================================================================
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
	struct timespec ts = {0, 100000000L };

int main(int argc, char* argv[]){


	time_t parkTime = time(NULL);
	struct tm tm = *localtime(&parkTime);


	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	parktime();

	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	int source;   /* rank of sender */
	int dest;     /* rank of receiver */
	int tag=0;    /* tag for messages */
	char message[100];        /* storage for message */
	MPI_Status status ;   /* return status for receive */
	
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	MPI_Comm_size(MPI_COMM_WORLD, &p); 
	
	

	MPI_Finalize(); 
	
	
	return 0;
}

int parktime(){

	int i,j;
	int horasDia = 24;
	int segundosHora = 60;

	for(i = 9; i < horasDia; i++){
		for(j = 0; j < segundosHora; j++){
			 nanosleep(&ts,NULL);
			printf("%d:%d\n",i,j);
		}

	}

	return 0;
}
//Esto es una prueba de sincronia my niggasssss




