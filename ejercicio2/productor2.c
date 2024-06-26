#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define N 8

int *buffer;

int produce_item() {
    return rand() % 11; // Corrección para generar un entero aleatorio entre 0 y 10
}

void insert_item(int item) {
    buffer[N]--; // Ajuste para comportamiento LIFO
    buffer[buffer[N]] = item;
    printf("Producido el valor %d en la posicion %d\n", item, buffer[N]);
}

int main() {
    int item, fich;
    srand(time(NULL));

    if ((fich = open("buffer", O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IROTH)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }

    ftruncate(fich, (N+1)*sizeof(int));

    buffer = mmap(NULL, (N+1)*sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, fich, 0);
    if (buffer == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }

    buffer[N] = N;

    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");
    sem_t *vacias = sem_open("VACIAS", O_CREAT, 0700, N);
    sem_t *llenas = sem_open("LLENAS", O_CREAT, 0700, 0);
    sem_t *mutex = sem_open("MUTEX", O_CREAT, 0700, 1);

    for (int i = 0; i < 100; i++) {
        item = produce_item();

        sem_wait(vacias);
        sem_wait(mutex);

        insert_item(item);

        sem_post(mutex);
        sem_post(llenas);

        sleep(rand() % 4);
        sleep(2);
    }

    munmap(buffer, (N+1)*sizeof(int));
    close(fich);
    sem_close(vacias);
    sem_close(llenas);
    sem_close(mutex);
    sem_unlink("VACIAS");
    sem_unlink("LLENAS");
    sem_unlink("MUTEX");

    return 0;
}
