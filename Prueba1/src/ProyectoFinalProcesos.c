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
#define minutosIncioDia 540  //9hrs
#define minutosDelDia 1380   //23hrs

	struct timespec ts = {0, 100000000L };
	int cupoFeria = 100;

int main(int argc, char* argv[]){


//	time_t parkTime = time(NULL);
//	struct tm tm = *localtime(&parkTime);
//
//
//	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);


	int  tid; /* rank of process */
	int  p;       /* number of processes */
	int horaActual = 0;

	MPI_Status status ;   /* return status for receive */
	
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &tid);

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	
	//Tiempo
	if(tid == 0){
		int i;
		int minutosFeria; //Horario de 9 a 23hr --- 540min a 1380min

		for(i = minutosIncioDia; i < minutosDelDia; i++){
				MPI_Bcast(&i,1,MPI_INT,0,MPI_COMM_WORLD);
				printf("%d:%d\n",i/60,i%60);
			}
	}
	//Pasillos = 0  Tiempo de estancia en pasillos 2 min
	if(tid == 1){
		int cupo[100];
		MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Recv(cupo,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,status);

		//

	}
	//Montana = 1 Duracion de juego
	if(tid == 2){
		int fila[15];
		int cupo[8];
		MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);

	}
	//caballo = 2
	if(tid == 3){
		int fila[20];
		int cupo[10];
		MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);


	}else{ //Todos los demas

		int horaLlegada = generadorHoraDeLLegada();
		int queHacer;

		MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);


		if(cupoFeria > 0){
			cupoFeria--;
			if(horaLlegada == horaActual){ //Llego a la feria
				do{
					queHacer = DondeIr(); //Decido que hacer
					printf("Voy a ir %d\n",queHacer);
					if(queHacer == 4){break;}//Evita que mande mensaje a tid = 4 y se rompa esta madre
					MPI_Send(&tid,1,MPI_INT,queHacer,100,MPI_COMM_WORLD);
					//Esperando que el juego te libere(Esperando el send del juego)
					MPI_Recv(&tid,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,status);
				}while(queHacer != 4);
				cupoFeria++; //Estoy saliendo de la feria
			}
		}else{
			//Codigo que hace que espere
		}
	}
	
	

	MPI_Finalize(); 
	
	
	return 0;

}
////////////////////////////////////////////////////
//AQUI TERMINA MAIN////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


int generadorHoraLlegada(){

	srand(time(NULL));

	int horaLlegada = rand() % 781;

	return horaLlegada;

}

int dondeIr(){
	//1 PASILLO
	//2 MOTANA
	//3 CABALLO
	//4 IRSE
	srand(time(NULL));
	int dondeIr = (rand()% 4) + 1;

	return dondeIr;
}


