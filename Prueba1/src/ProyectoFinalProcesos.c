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


#define horaIncioDia 540  //9hrs
#define horaApertura 600  //10hrs
#define horaFinDia 1380   //23hrs

	struct timespec ts = {0, 100000000L };
	int cupoFeria = 100;

////DECLARACIÓN DE FUNCIONES////

int generarHoraLlegada(){

	srand(time(NULL));

	int horaLlegada = rand() % 781;

	return horaLlegada;

}

int dondeIr(){ //genera la descición de la gente de a donde ir
	//1 PASILLO
	//2 MOTANA
	//3 CABALLO
	//4 IRSE
	srand(time(NULL));
	int dondeIr = (rand()% 4) + 1;

	return dondeIr;
}

void reloj(int tid, int hora){ //prueba para saber si un proceso recibe la hora


	if(hora==540 || hora==1380)printf("soy la persona %d hora %d:00\n", tid, hora/60);

	return;

}

void inicializaArreglo(int arreglo[], int tam){ //llena los arreglos con ceros

	int i;
	for(i=0; i<tam; i++){
		arreglo[i]=0;
		printf("%d",arreglo[i]);


	}
}


////DECLARACIÓN DE FUNCIONES////

int main(int argc, char* argv[]){


	int  tid; /* rank of process */
	int  p;       /* number of processes */
	int horaActual;

	MPI_Status status ;   /* return status for receive */
	
	
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &tid);

	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	//Tiempo
	if(tid == 0){
		int i;

		for(i = horaIncioDia; i <= horaFinDia; i++){
			//horaActual=i;
			MPI_Bcast(&i,1,MPI_INT,0,MPI_COMM_WORLD);
			}
	}

	do{

		//Pasillos = 0  Tiempo de estancia en pasillos 2 min
		if(tid == 1){

			int cupo[100];
			inicializaArreglo(cupo, 100);

			MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);
			//MPI_Recv(cupo,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

			reloj(tid,horaActual);
			//

		}
		//Montana = 1 Duracion de juego
		if(tid == 2){
			int fila[15];
			int cupo[8];
			inicializaArreglo(fila, 15);
			inicializaArreglo(cupo, 8);

			MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);

			reloj(tid,horaActual);
		}
		//caballo = 2
		if(tid == 3){
			int fila[20];
			int cupo[10];
			inicializaArreglo(fila, 20);
			inicializaArreglo(cupo, 10);

			MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);

			reloj(tid,horaActual);
		}
		//Todos los demas
		else{
			int horaLlegada = generarHoraLlegada();
			int queHacer;

			MPI_Bcast(&horaActual,1,MPI_INT,0,MPI_COMM_WORLD);
	
			reloj(tid,horaActual);
	
			/*if(cupoFeria > 0){
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
			}*/
		}

	}while(horaActual<finDia);

	MPI_Finalize(); 
	
	
	return 0;

}


