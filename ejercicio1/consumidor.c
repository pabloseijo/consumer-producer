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

void consume_item();

int main(){

	int i, fich;  
	
	//-------------------- COMPROBACIONES Y DECLARACIONES --------------------
    // Se abre el fichero del buffer, si no existe se crea
	if( (fich=open("buffer.txt", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO)) == -1){
		printf("ERROR: la funcion open tuvo un problema (linea 21)\n");
		exit(0);
	} 
	
	//Se proyecta el fichero en memoria
    /**
     * void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
     * addr: dirección de memoria donde se quiere proyectar el fichero. Si es NULL, el sistema elige la dirección.
     * length: tamaño de la proyección en bytes. 
     * prot: protección de la memoria. PROT_READ, PROT_WRITE, PROT_EXEC, PROT_NONE.
     * flags: MAP_SHARED, MAP_PRIVATE, MAP_FIXED, MAP_ANONYMOUS, MAP_FIXED_NOREPLACE.
     * fd: descriptor del fichero.
     * offset: desplazamiento en el fichero.
    */
	if( (buffer = mmap(NULL, (N + 1) *sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich, 0)) == MAP_FAILED){
        printf("ERROR: la funcion mmap produjo un error (linea 36)\n");
        exit(0);
    }

	srand(time(NULL)); // Semilla para la función rand()

    //-------------------- CONSUMO DE ELEMENTOS --------------------
		
    // Metemos la espera activa tal que mientras el buffer esté vacío, el consumidor no pueda consumir
	for(i = 0; i < 100; ++i){
		while(buffer[N] == 0);
		consume_item();
	}
	
	//Se elimina la proyeccion en memoria
    /**
     * int munmap(void *addr, size_t length);
     * addr: dirección de memoria donde se quiere proyectar el fichero. Si es NULL, el sistema elige la dirección.
     * length: tamaño de la proyección en bytes.
     * La función devuelve 0 si se ha realizado correctamente y -1 si ha habido algún error.
    */
	if(munmap(buffer, (N+1)*sizeof(int))!=0){
        printf("ERROR: la funcion munmap produjo un error (linea 50)\n");
        exit(0);
    }
        
    //Se cierra el fichero
    if(close(fich)!=0){
        printf("ERROR: la funcion close produjo un error (linea 57)\n");
        exit(0);
    } 
    
    return 0;
}

void consume_item() {
    // Verificar si hay elementos para consumir
    if (buffer[N] > 0) {
        // Decrementar la cuenta de elementos en el buffer
        buffer[N]--;

        // Introducimos un retardo aleatorio para simular el tiempo de consumo
        sleep(rand() % 5);

        int posConsumido = buffer[N];
        int consumido = buffer[posConsumido];

        // Marcamos la posición como vacía o consumida
        buffer[posConsumido] = -1;

        // Imprimir información del elemento consumido
        printf("Valor %d consumido (Posicion: %d)\n", consumido, posConsumido);

    } else printf("No hay elementos para consumir.\n");
}


