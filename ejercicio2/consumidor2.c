#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define N 8

int *buffer;

void consume_item() {
    int position = buffer[N];
    int consumed = buffer[position];
    int sum = consumed;
    for (int i = 0; i < position; i++) {
        sum += buffer[i]; // Suma de todos los valores en el buffer
    }
    buffer[N]++; // Ajuste para comportamiento LIFO
    printf("Consumido el valor %d, suma de valores en el buffer: %d\n", consumed, sum);
}

int main() {
    int fich;
    srand(time(NULL));

    // Manejo del archivo del buffer
    if ((fich=open("buffer", O_RDWR)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }

    // Mapeo de memoria
    if ((buffer=mmap(NULL, (N+1)*sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich, 0)) == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }

    // Apertura de semáforos existentes
    sem_t *llenas = sem_open("LLENAS", 0);
    sem_t *vacias = sem_open("VACIAS", 0);
    sem_t *mutex = sem_open("MUTEX", 0);

    for (int i = 0; i < 100; i++) {
        sem_wait(llenas);
        sem_wait(mutex);

        consume_item();

        sem_post(mutex);
        sem_post(vacias);

        sleep(rand() % 4); // Espera aleatoria entre 0 y 3 segundos
    }

    // Limpieza
    munmap(buffer, (N+1)*sizeof(int));
    close(fich);
    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");

    return 0;
}
