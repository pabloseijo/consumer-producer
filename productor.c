#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <time.h>

#define N 8

int *buffer;

int produce_item();
void insert_item(int item);

int main(){
	int i, fich;
	
    //-------------------- COMPROBACIONES Y DECLARACIONES --------------------

	//Se abre el fichero del buffer, si no existe se crea
	if( (fich = open("buffer.txt", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
		printf("ERROR: la funcion open tuvo un problema (linea 23)\n");
		exit(0);
	}
	  
	//Se trunca el fichero para que tenga tamañao para los N elementos del buffer mas la cuenta   	
	ftruncate(fich, (N + 1) * sizeof(int));

	//Se proyecta el fichero en memoria
	if( (buffer = mmap(NULL, (N + 1) * sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich, 0)) == MAP_FAILED){
        printf("ERROR: la funcion mmap produjo un error (linea 32)\n");
        exit(0);
    }
	
    //Inicializamos el buffer
	buffer[N]=0;
	
    //Semilla para la función rand()
	srand(time(NULL));

    //-------------------- PRODUCCION DE ELEMENTOS --------------------
	
    //Metemos la espera activa tal que mientras el buffer esté lleno, el productor no pueda producir
	for(i=0; i<100; i++){
		while(buffer[N]==N);	
		//item=produce_item();
		insert_item(i);
	}
	
	//Se elimina la proyeccion en memoria
	if(munmap(buffer, (N+1)*sizeof(int))!=0){
            printf("ERROR: munmap\n");
            exit(0);
        }
        
        //Se cierra el fichero
        if(close(fich)!=0){
            printf("ERROR: close\n");
            exit(0);
        }
}


int produce_item(){
	return buffer[N];
}

void insert_item(int item){
	buffer[buffer[N]]=item;
	printf("Introduce %d en la posicion %d\n", item, buffer[N]);
	sleep(rand()%5);
	buffer[N]++;
}

