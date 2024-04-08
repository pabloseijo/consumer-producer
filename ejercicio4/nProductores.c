#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 8

int *buffer;

int i;
int produce_item();
void insert_item(int item);

int main() {

	int item, f, bufferCasiVacio=0, j, cuenta;	
	
	printf("\nIntroduzca el numero de productores: ");
	scanf( "%d", &cuenta);
	
	pid_t padre=getpid(), hijos[cuenta];
	
	// se borran los posibles semáforos previos antes de crear los nuevos
	sem_unlink("VACIAS");
	sem_unlink("LLENAS");
	sem_unlink("MUTEX");
	
	//Se inicializan los semaforos
	sem_t *vacias = sem_open("VACIAS",O_CREAT,0700,N);
	sem_t *llenas = sem_open("LLENAS",O_CREAT,0700,0);
	sem_t *mutex = sem_open("MUTEX",O_CREAT,0700,1);
	
	srand(time(NULL));

	// Elimina el archivo existente para asegurar un inicio limpio
	unlink("buffer");

	// Abre (y crea) el archivo, truncando si ya existía
	if ((f = open("buffer", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
		perror("Error al abrir el fichero del buffer");
		exit(EXIT_FAILURE);
	}

	// Configura el tamaño del archivo
	ftruncate(f, (N+1)*sizeof(int));

	// Mapea el archivo en memoria
	buffer = mmap(NULL, (N+1)*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
	if (buffer == MAP_FAILED) {
		perror("Error en la función mmap");
		exit(EXIT_FAILURE);
	}

		
	buffer[N] = N - 1;
		
	// se crean los procesos hijos
	for(j = 0; j < cuenta; j++){
		if(getpid()==padre){
			hijos[j]=fork();
			if(hijos[j]<0) printf("\t\tError al crear el hijo %d\n", j);
		}
	}
	
	if(getpid()!=padre){
		
		for(i=0; i<100; i++){
			item=produce_item();
			//Se espera a que vacias > 0
			sem_wait(vacias);
			//Se espera a entrar en la region critica
			sem_wait(mutex);
			
			//Si el buffer esta casi vacio, produce mas rapido
			if(buffer[N] < 3){
				bufferCasiVacio = 0;
			}
				
			insert_item(item);
			
			//Si el buffer esta casi lleno, produce mas lento
			if(buffer[N] > N-3){
				bufferCasiVacio = 1;
			}
				
			//Se libera la region critica
			sem_post(mutex);
			//Se incrementa llenas
			sem_post(llenas);
				
			if(bufferCasiVacio == 0){
				sleep(rand()%5);
			}	
			else{
				sleep(rand()%10);
			}
			
			sleep(3);
					
		}
		
	} else {
        //El proceso padre espera a que acaben los hijos
    	for(j=0; j<cuenta; j++){
     		waitpid(hijos[j], 0, 0);
      	}
        	
		//Se elimina la proyeccion en memoria
		if(munmap(buffer, (N+1)*sizeof(int))!=0){
		    printf("ERROR: munmap\n");
		    exit(0);
		}
		
        //Se cierra el fichero
		if(close(f)!=0){
		    printf("ERROR: close\n");
		    exit(0);
		}
		
		//Se borran los semaforos
		sem_unlink("VACIAS");
		sem_unlink("LLENAS");
		sem_unlink("MUTEX");
		
		printf("Acabaron todos los productores\n");
       
    }
}



int produce_item() {
    return rand() % 11; // Genera un número aleatorio entre 0 y 10
}

void insert_item(int item) {
    buffer[buffer[N]] = item; // Inserta el item en la posición indicada por buffer[N]
    printf("Productor %d: produce el valor %d en la posicion %d\n", getpid(), item, buffer[N]);
    buffer[N]--; // Decrementa el índice para el próximo insert
}

