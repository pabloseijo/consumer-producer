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
    if (buffer[N] < N) { // Verifica si hay elementos para consumir
        int position = buffer[N]; // Obtiene la posición del elemento a consumir
        int consumed = buffer[position]; // Consumir el elemento en la posición actual
        int sum = consumed;
        
        // Sumar los elementos desde la posición actual hasta el final del buffer
        for (int i = position + 1; i < N; i++) {
            sum += buffer[i];
        }
        
        printf("Consumido el valor %d de la posicion %d, suma de valores en el buffer: %d\n", consumed, position, sum);
        buffer[N]++; // Ajusta el indicador de posición para el próximo elemento a consumir
    } else {
        printf("Buffer vacío, no se puede consumir.\n");
    }
}


int main() {
    int fich;
    srand(time(NULL));

    if ((fich = open("buffer", O_RDWR)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }

    buffer = mmap(NULL, (N+1)*sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich, 0);
    if (buffer == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *llenas = sem_open("LLENAS", 0);
    sem_t *vacias = sem_open("VACIAS", 0);
    sem_t *mutex = sem_open("MUTEX", 0);

    for (int i = 0; i < 100; i++) {
        sem_wait(llenas);
        sem_wait(mutex);

        consume_item();

        sem_post(mutex);
        sem_post(vacias);

        sleep(rand() % 4);
        sleep(2);
    }

    munmap(buffer, (N+1)*sizeof(int));
    close(fich);
    sem_close(llenas);
    sem_close(vacias);
    sem_close(mutex);
    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");

    return 0;
}
