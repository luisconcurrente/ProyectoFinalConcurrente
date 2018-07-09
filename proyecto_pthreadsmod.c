
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

#define	NUM_DE_CLIENTES		  150
#define NUM_CLIENTES_ADMITIDOS	  100

//Definimos horas importantes en minutos
#define HORA_DE_INICIO_SIMULACION 480//Minuto  8:00
#define HORA_DE_APERTURA          600//10 Hrs
#define HORA_DEL_CIERRE		  1380//23 Hrs
#define MINUTO		          100000// 
#define	ABIERTO			  111
#define CERRADO 		  222




//Variables del carrusel
#define FILA_CARRUSEL			20//Cantidad de clientes maxima que pueden estar formados en la fila
#define NUM_LUGARES_CABALLOS		10//Cantidad de lugares disponibles del juego
#define TIEMPO_DE_JUEGO_CABALLOS	1//Numero de minutos que esta en funcionamiento el juego una vez que comience

//variables de la Montaña Rusa
#define FILA_RUSA				15//Cantidad de clientes maxima que pueden estar formados en la fila
#define NUM_LUGARES_RUSA			8//Cantidad de lugares disponibles del juego
#define TIEMPO_DE_JUEGO_RUSA		2//Numero de minutos que esta en funcionamiento el juego una vez que comience

//Estados
#define PASILLO						0//Estados del Cliente
#define SOBRE_CABALLOS				1//Estados del Cliente
#define SOBRE_RUSA					2//Estados del Cliente
#define MAREADO						3//Estados del Cliente

//Desiciones
#define DECIDE_PASILL0				0//Desiciones posibles
#define DECIDE_CABALLOS				1//Desiciones posibles
#define DECIDE_RUSA					2//Desiciones posibles

//Variables compartidas
int EstadoParque=CERRADO;
int ClientesDentro=0;


int CabFila=0;
int RusaFila=0;
int CabOcupados=0;
int RusaOcupados=0;
int TiempoTranscurrido=HORA_DE_INICIO_SIMULACION;
int HorasTrans=0;
int MinutosTrans=0;


//Declaracion de herramientas de control
//Declaración de candados para secciones criticas y barreras de las filas y entrada
sem_t    critic_Carrusel,
	 critic_Mont,
         critic_Entrada,
	 barFilaC,
         barFilaR,
         barEntrada;

pthread_t HilosClientes[NUM_DE_CLIENTES];//Arreglo de clientes

pthread_t hiloControl_Caballos,
          hiloControl_Rusa,
          hiloControlDelParque;

pthread_cond_t Apertura;// Bloqueo para evitar que los clientes entren antes de que el parque este ABIERTO

pthread_mutex_t mutexApertura;//Mutex necesario para la barrera de Apertura, ue no entren mas de la capacidad



/*

*Este metodo regresa EN_CARRUSEL si logra formarse en la fila,  en otrr caso regresa PASILLO 
*La validacion se realiza en una seccion critica para que solo un hilo lo haga con un semaforo

*/
int IntentaSubirCab(int id){
	sem_wait(&critic_Carrusel);
		if(CabFila<FILA_CARRUSEL){
			CabFila++;
			printf("(%d:%d)  Soy el cliente %d estoy formado en el carrusel, en el lugar %d \n",HorasTrans, MinutosTrans, id, CabFila);
			sem_post(&critic_Carrusel);
			return(SOBRE_CABALLOS);
		}
		else{
			printf("(%d:%d)   Soy el cliente %d no pude formarme en el carrusel , fila con %d clientes,regreso a pasilllos\n",HorasTrans, MinutosTrans, id, CabFila);
			sem_post(&critic_Carrusel);
			return(PASILLO);
		}
}

/*
*Esto metodo realiza lo mismo que el metodo anterior solo que con la montaña rusa
*
*/
int IntentaSubirRusa(int id){
	sem_wait(&critic_Mont);//seccion critica
		if(RusaFila<FILA_RUSA){
			RusaFila++;
			printf("(%d:%d)  Soy el cliente %d estoy formado en la Montaña Rusa, en el lugar %d \n",HorasTrans, MinutosTrans, id, RusaFila);
			sem_post(&critic_Mont);
			return(SOBRE_RUSA);
		}
		else{
			printf("(%d:%d)   Soy el cliente %d no pude formarme en la Montaña Rusa, fila con %d clientes,regreso a pasilllos\n", HorasTrans, MinutosTrans, id, RusaFila);
			sem_post(&critic_Mont);
			return(PASILLO);
		}
}
/*
*Este metodo controlo los estados del parque, funciona todo el tiempo
*Este va a iniciar la hora, cuando sea la hora de abrir cambia el estado del parque
*cuando llegue a la hora del cierre vuelve a cambiar el estado del parque  
*despues va liberando los hilos que sigan formados
*/

void* ControlDelParque(){
	pthread_cond_init(&Apertura, NULL);
	printf("Control del parque encendido\n");
	while(TiempoTranscurrido<HORA_DEL_CIERRE){
	TiempoTranscurrido++;//Incrementa una unidad de tiempo en minutos
	MinutosTrans=TiempoTranscurrido%60;//Calcula los minutos
	HorasTrans=TiempoTranscurrido/60;//Calcula las horas
	usleep(MINUTO);//Espera antes de volver a calcular el tiempo que pasa en proporcion al tamaño del minuto

		if (TiempoTranscurrido==HORA_DE_APERTURA)
		{	
			EstadoParque=ABIERTO;
			printf("(%d:%d) \E[0;31m ¡¡¡¡¡¡¡¡¡¡¡¡ LA FERIA ESTA ABIERTA!!!!!!!!!!!!!!!!!!\E[00m \n", HorasTrans, MinutosTrans);
			pthread_cond_broadcast(&Apertura);//Libera todos los hilos que estaban en la condicion de Apertura
			
		}
	}
	

	printf("(%d:%d)\E[0;31m    °°°°°°°ES LA HORA DEL CIERRE°°°°°°°\E[00m \n", HorasTrans, MinutosTrans);
	printf("(%d:%d)\E[0;31m   °°°°°°°LA FERIA CERRARA°°°°°°°°°°°° \E[00m \n", HorasTrans, MinutosTrans);
	EstadoParque=CERRADO;//Cambia el estado del parque
	printf("(%d:%d)\E[0;31m ¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡LA FERIA ESTA CERRADA!!!!!!!!!!!!!!!!!\E[00m \n",HorasTrans, MinutosTrans);


	//Una vez que la simulacion termina libera a los que estan formados si es que no han logrado salir de alguna fila
	for (int i = 0; i <FILA_RUSA; ++i)
	{
		sem_post(&barFilaR);
	}
	for (int i = 0; i < FILA_CARRUSEL; ++i)
	{
		sem_post(&barFilaC);
	}


	pthread_exit(NULL);
	//Termina el hilo ControlDelParque

}

/*
*ESte metodo simula a los clientes
*si el parque esta cerrado espera en la barrera de la entrada
*si no entra al parque, pasa por el samaforo de la entrada que disminuye conforme pasan
*en una seccion critica de aumenta el numero de clientes dentro
*el cliente inicia con cero vueltas y en los pasillos
*debe decidir a que juego subirse, con un random entre 1 y2
*despues ve si puede formarse
*si se puede subir cambia su estado y aumneta sus vueltas dadas
*si esta en los pasillos espera
*despues checa si las vueltas son mayores a nueve cambia su estado a mareado y sale del parque
*
*/
void* Cliente(void* IDCliente){

	usleep((rand()%480*MINUTO)+HORA_DE_INICIO_SIMULACION);//Detiene aleatoreamente a los clientes hasta 8 Hrs = 480 min  maximo
	int id = (long)IDCliente;

	printf("(%d:%d)\E[0;35m    El Cliente %d llego a la feria\E[00m\n",HorasTrans, MinutosTrans, id );


	if (EstadoParque==CERRADO)
	{
		printf("(%d:%d)\E[1;35m   Soy el Cliente %d la feria esta CERRADA, espero en la entrada\E[00m \n",HorasTrans, MinutosTrans, id);
		pthread_cond_wait(&Apertura, &mutexApertura);
		pthread_mutex_unlock(&mutexApertura);

	}

	sem_wait(&barEntrada);
	printf("(%d:%d)\E[0;35m   Soy el Cliente %d  estoy Entrando a la feria \E[00m \n", HorasTrans, MinutosTrans, id);

	sem_wait(&critic_Entrada);
	ClientesDentro++;//Incrementa los clientes dentro del parque
	sem_post(&critic_Entrada);

	int num_vueltas=0;
	int estado=PASILLO;
	int decision;

	//Mientras el TiempoTranscurrido sea menor a 20 minutos antes de la HORA_DEL_CIERRE , o que el estado del cliente sea distinto de MAREADO entonces

	while(TiempoTranscurrido<HORA_DEL_CIERRE-25 && estado!=MAREADO){
		decision=(rand()%2)+1;

		if (decision==DECIDE_CABALLOS){
			estado=IntentaSubirCab(id);//regrea si esta en el carrusel o si regresa a los pasillos	
		}
		if(decision==DECIDE_RUSA){
			estado=IntentaSubirRusa(id);//regrea si esta en la montaña o si regresa a los pasillos	
		} 

		
		if (estado==SOBRE_CABALLOS)
		{	
			sem_wait(&barFilaC);

			usleep(MINUTO*0.1);

			printf("(%d:%d)   Soy el cliente %d, El carrusel se esta moviendo\n",HorasTrans, MinutosTrans, id);
			usleep(TIEMPO_DE_JUEGO_CABALLOS*MINUTO);//proporcion de tiempo que esperara en minutos repecto al tamaño del minuto
			usleep(MINUTO*0.1);//Retraso para esperar a que el tiempo se actualize correctamente
			printf("(%d:%d)   Soy el cliente %d bajo del Carrusel y regreso a pasillos\n",HorasTrans, MinutosTrans, id );
			estado=PASILLO;
			num_vueltas++;

		}

		if(estado==SOBRE_RUSA){

			sem_wait(&barFilaR);

			usleep(MINUTO*0.1);

			printf("(%d:%d)   Soy el cliente %d, La Montaña rusa se esta moviendo\n",HorasTrans, MinutosTrans, id);
			usleep(TIEMPO_DE_JUEGO_RUSA*MINUTO);
			usleep(MINUTO*0.1);
			printf("(%d:%d)   Soy el cliente %d bajo de la Montaña y regreso a pasillos\n",HorasTrans, MinutosTrans, id );
			estado=PASILLO;
			num_vueltas++;
		}
		
		if (estado==PASILLO)
		{
			printf("(%d:%d)\E[0;36m   Soy Cliente %d en PASILLO esperando\E[00m \n",HorasTrans, MinutosTrans, id);
			usleep(MINUTO*2);
		}

		if(num_vueltas>9){
			estado=MAREADO;//Cambia su estado para salir del while
			printf("(%d:%d)\E[1;33m    Soy el cliente %d estoy MAREADO ya me voy\E[00m\n",HorasTrans, MinutosTrans, id );
			sem_post(&barEntrada);//Al salir libera un cliente que estaba esperando por CAPACIDAD_DEL_PARQUE
		}
	}
	
	sem_wait(&critic_Entrada);
		printf("(%d:%d)\E[0;33m Cliente %d desalojando el parque\E[00m \n",HorasTrans, MinutosTrans, id );
		ClientesDentro--;//Disminuye la cnatidad de cliente dentro abandona 
	sem_post(&critic_Entrada);
	pthread_exit(NULL);//Termina el hilo de cliente
}

/*
*mientras no sea la hora del cierre
*inicia el juego si por lo menos hya uno formado
*En una seccion criticaintercambio enree clientes de la fila y los ligares en el juego
    *conforme pasan libero el semaforo de la fila para que otros pasen
*inicio el juego
*/
void* Control_Caballos(){
	printf("Administrador Caballos creado\n");
	while(TiempoTranscurrido<HORA_DEL_CIERRE){
		if(TiempoTranscurrido%10 == (0%10) && CabFila!=0 ){
			sem_wait(&critic_Carrusel);
			while(CabOcupados<NUM_LUGARES_CABALLOS && CabFila>0)
			{	
				CabFila--; 
				CabOcupados++;
				sem_post(&barFilaC);
			}
			sem_post(&critic_Carrusel);
			
			sem_wait(&critic_Carrusel);
			printf("(%d:%d) \E[0;32m°°°°°°°°INICIA JUEGO CABALLOS, CON UN TOTAL DE %d LUGARES OCUPADOS°°°°°°\E[00m\n", HorasTrans, MinutosTrans, CabOcupados );
			CabOcupados=0;
			sem_post(&critic_Carrusel);
			
			usleep(TIEMPO_DE_JUEGO_CABALLOS*MINUTO);
			printf("(%d:%d)\E[0;32m °°°°°°°°FIN DEL JUEGO CABALLOS, LOS CLIENTES ESTAN DESOCUPANDO DE JUEGO°°°°°°\E[00m\n",HorasTrans, MinutosTrans);
		}
	}

	pthread_exit(NULL);
}

/*
*Lo mismo que el anterior para la montaña rusa
*/
void* Control_Rusa(){
	printf("Administrador montaña creado\n");
	while(TiempoTranscurrido<HORA_DEL_CIERRE){
	if (TiempoTranscurrido%10==(5%10) && RusaFila!=0){
			sem_wait(&critic_Mont);
			while(RusaOcupados<NUM_LUGARES_RUSA && RusaFila>0)
			{
				RusaFila--;
				RusaOcupados++;
				sem_post(&barFilaR);
			}
			sem_post(&critic_Mont);
			
			sem_wait(&critic_Mont);
			printf("(%d:%d)\E[0;34m °°°°°°°°INICIA JUEGO MONTAÑA RUSA, CON UN TOTAL DE %d LUGARES OCUPADOS°°°°°°° \E[00m \n", HorasTrans, MinutosTrans, RusaOcupados);
			RusaOcupados=0;
			sem_post(&critic_Mont);

			usleep(TIEMPO_DE_JUEGO_RUSA*MINUTO);
			printf("(%d:%d)\E[0;34m°°°°°°°FIN DEL JUEGO MONTANIA RUSA, LOS CLIENTES ESTAN DESOCUPANDO DE JUEGO°°°°°°\E[00m \n", HorasTrans, MinutosTrans);
		}
	}

	pthread_exit(NULL);
}



int main(int argc, char const *argv[])
{	
	printf("\E[1;31m ******************INICIA EL PROGRMA*********************\E[00m \n");
        srand(time(NULL));//Funcion que prepara para generar numeros aleatorios
	//pthread_cond_init(&Apertura, NULL);
	pthread_mutex_init(&mutexApertura, NULL);
	sem_init(&critic_Carrusel, 0, 1);
	sem_init(&critic_Mont, 0, 1);
	sem_init(&critic_Entrada, 0, 1);
	sem_init(&barFilaC, 0, 0);
	sem_init(&barFilaR, 0, 0);
	sem_init(&barEntrada, 0, NUM_CLIENTES_ADMITIDOS);
	//Inicia los hilos administradores de los juegos
	pthread_create(&hiloControl_Caballos, NULL, Control_Caballos, NULL);
	pthread_create(&hiloControl_Rusa, NULL, Control_Rusa, NULL);
	//Inicia los hilos de los clintes
	for (long i = 0; i < NUM_DE_CLIENTES; ++i)
	{
		pthread_create(&HilosClientes[i], NULL, Cliente, (void*)i);
	}
	//Inicia el hilo de control del parque
	pthread_create(&hiloControlDelParque, NULL, ControlDelParque, NULL);

	pthread_cond_destroy(&Apertura);
	for (int i = 0; i < NUM_DE_CLIENTES; ++i)
	{
		pthread_join(HilosClientes[i], NULL);
	}
	pthread_join(hiloControlDelParque, NULL);
	pthread_join(hiloControl_Caballos, NULL);
	pthread_join(hiloControl_Rusa, NULL);

	printf("\E[1;31m ************************FIN DEL PROGRAMA*********************************\E[00m");

	return 0;
}

