#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h> 

#define N 8

int *buffer1, *buffer2;


int produce_item() {
    return rand() % 11; // Produce un número aleatorio entre 0 y 10.
}

void insert_item(int item) {
    if (buffer2[N] >= 0) { // Asegura que hay espacio en el buffer.
        buffer2[buffer2[N]] = item; // Inserta el item en la posición actual.
        printf("Producido el valor %d en la posicion %d del buffer2\n", item, buffer2[N]);
        buffer2[N]--; // Decrementa el índice para el próximo item, moviéndose hacia el inicio del buffer.
    }
}

void consume_item(int *buffer) {
    if (buffer[N] < N-1) { // Verifica si hay elementos para consumir.
        buffer[N]++; // Incrementa primero para moverse a la posición correcta.
        int position = buffer[N];
        int consumed = buffer[position];
        printf("Consumido el valor %d de la posicion %d del buffer1\n", consumed, position);
    } else {
        printf("Buffer vacío, no se puede consumir.\n");
    }
}


int main() {
    int fich1, fich2, item;
    srand(time(NULL));

    //--------------- Inicialización de buffer1 y buffer2 ---------------//

    // Elimina el archivo existente para asegurar un inicio limpio
	unlink("buffer2");

    // Inicialización y mapeo de buffer1 para producción
    fich2 = open("buffer2", O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IROTH);
    ftruncate(fich2, (N+1)*sizeof(int));
    buffer2 = mmap(NULL, (N+1)*sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich2, 0);
    buffer2[N] = N - 1; // Inicializa el índice al final del buffer.

    // Inicialización y mapeo de buffer2 para consumo
    // Apertura o creación del archivo del buffer
    if ((fich1 = open("buffer1", O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }
    
    // Mapeo del archivo del buffer en memoria
    buffer1 = mmap(NULL, (N + 1) * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fich1, 0);
    if (buffer1 == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }
    //------------------ Inicialización de semáforos ------------------//

    // Semáforos para buffer2
    sem_unlink("VACIAS2");
    sem_unlink("LLENAS2");
    sem_unlink("MUTEX2");
    sem_t *vacias2 = sem_open("VACIAS2", O_CREAT, 0700, N);
    sem_t *llenas2 = sem_open("LLENAS2", O_CREAT, 0700, 0);
    sem_t *mutex2 = sem_open("MUTEX2", O_CREAT, 0700, 1);

    // Semáforos para buffer1
    sem_t *vacias1 = sem_open("VACIAS1", O_CREAT, 0700, N);
    sem_t *llenas1 = sem_open("LLENAS1", O_CREAT, 0700, 0);
    sem_t *mutex1 = sem_open("MUTEX1", O_CREAT, 0700, 1);

    //------------------ Producción y consumo de items ------------------//

    for (int i = 0; i < 100; i++) {

        sem_wait(vacias2); // Espera tener espacio en buffer1
        sem_wait(mutex2); // Espera acceso exclusivo a buffer1
        
        item = produce_item();
        insert_item(item); // Corregido para insertar en buffer1

        sem_post(mutex2); // Libera acceso a buffer1
        sem_post(llenas2); // Indica que hay un nuevo item en buffer1

        // Simula tiempo de producción/consumo
        sleep(rand() % 5); 

        // Consumo de buffer2 
        sem_wait(llenas1); // Espera que haya items en buffer2        
        sem_wait(mutex1); // Espera acceso exclusivo a buffer2

        consume_item(buffer1); // Corregido para consumir de buffer2

        sem_post(mutex1); // Libera acceso a buffer2
        sem_post(vacias1); // Indica que hay espacio en buffer2

        sleep(rand() % 5);
    }

    //------------------ Liberación de recursos ------------------//

    munmap(buffer1, (N+1)*sizeof(int));
    munmap(buffer2, (N+1)*sizeof(int));

    close(fich1);
    close(fich2);

    sem_close(vacias1);
    sem_close(llenas1);
    sem_close(mutex1);

    sem_close(vacias2);
    sem_close(llenas2);
    sem_close(mutex2);

    sem_unlink("VACIAS1");
    sem_unlink("LLENAS1");
    sem_unlink("MUTEX1");
    
    sem_unlink("VACIAS2");
    sem_unlink("LLENAS2");
    sem_unlink("MUTEX2");

    return 0;
}