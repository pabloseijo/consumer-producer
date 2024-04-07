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

sem_t *llenas, *vacias, *mutex;

// Función para producir un elemento
int produce_item() {
    return rand() % 11; // Genera un número aleatorio entre 0 y 10
}

// Función para insertar un elemento en el buffer
void insert_item(int item) {
    buffer[--buffer[N]] = item; // Pre-decremento para insertar en la posición correcta
    printf("Producido: %d en posición: %d\n", item, buffer[N]);
}

// Función para consumir un elemento del buffer
void remove_item() {
    int item = buffer[buffer[N]];
    int sum = item;
    for (int i = buffer[N] + 1; i < N; i++) {
        sum += buffer[i];
    }
    printf("Consumido: %d, Suma en el buffer: %d\n", item, sum);
    buffer[N]++; // Incrementamos después de consumir para mover el índice hacia el siguiente elemento disponible
}

// Hilo productor
void *productor(void *arg) {
    int item;
    for (int i = 0; i < 100; i++) {

        item = produce_item();

        sem_wait(vacias);
        sem_wait(mutex);

        insert_item(item);

        sem_post(mutex);
        sem_post(llenas);

        sleep(rand() % 4); // Tiempo aleatorio para simular velocidad de producción
    }
    return NULL;
}

// Hilo consumidor 
void *consumidor(void *arg) {
    for (int i = 0; i < 100; i++) {
        sem_wait(llenas);
        sem_wait(mutex);

        remove_item();

        sem_post(mutex);
        sem_post(vacias);

        sleep(rand() % 4); // Tiempo aleatorio para simular velocidad de consumo
    }
    return NULL;
}

int main() {
    
    srand(time(NULL));
    pthread_t hilo1, hilo2;
    int fich;

    // Inicialización del buffer y semáforos
    if ((fich = open("buffer", O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }

    ftruncate(fich, (N + 1) * sizeof(int));

    buffer = mmap(NULL, (N + 1) * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fich, 0);

    if (buffer == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }
    buffer[N] = N; // Posición inicial correcta para LIFO

    // Limpieza y creación de semáforos
    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");

    // Inicialización de semáforos
    vacias = sem_open("VACIAS", O_CREAT, 0700, N);
    llenas = sem_open("LLENAS", O_CREAT, 0700, 0);
    mutex = sem_open("MUTEX", O_CREAT, 0700, 1);

    // Creación de hilos
    pthread_create(&hilo1, NULL, productor, NULL);
    pthread_create(&hilo2, NULL, consumidor, NULL);

    // Espera a que los hilos terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    // Limpieza
    munmap(buffer, (N + 1) * sizeof(int));

    close(fich);

    sem_close(vacias);
    sem_close(llenas);
    sem_close(mutex);

    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");

    return 0;
}
