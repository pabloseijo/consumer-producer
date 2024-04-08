#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 8 // Tamaño del buffer

int *buffer;

void consume_item();

int main() {
    int f, j, cuenta;
    
    printf("\nIntroduzca el numero de consumidores: ");
    scanf("%d", &cuenta);
    
    pid_t padre = getpid(), hijos[cuenta];
    
    // Apertura o creación del archivo del buffer
    if ((f = open("buffer", O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        perror("ERROR: open");
        exit(EXIT_FAILURE);
    }
    
    // Mapeo del archivo del buffer en memoria
    buffer = mmap(NULL, (N + 1) * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, f, 0);
    if (buffer == MAP_FAILED) {
        perror("ERROR: mmap");
        exit(EXIT_FAILURE);
    }
    
    // Apertura de los semáforos (ya inicializados por el productor)
    sem_t *llenas = sem_open("LLENAS", 0);
    sem_t *vacias = sem_open("VACIAS", 0);
    sem_t *mutex = sem_open("MUTEX", 0);
    
    srand(time(NULL)); // Inicialización del generador de números aleatorios
    
    for (j = 0; j < cuenta; j++) {
        if (getpid() == padre) {
            hijos[j] = fork();
            
            if (hijos[j] < 0) {
                printf("\t\tError al crear el hijo %d\n", j);
            } else if (hijos[j] == 0) { // Proceso hijo consume
                for (int i = 0; i < 100; ++i) {
                    sem_wait(llenas);
                    sem_wait(mutex);
                    
                    consume_item();
                    
                    sem_post(mutex);
                    sem_post(vacias);
                    
                    sleep(rand() % 4); // Espera aleatoria para simular variaciones en la velocidad de consumo
                }
                exit(0);
            }
        }
    }
    
    if (getpid() == padre) {
        // El proceso padre espera a que todos los procesos hijos terminen
        for (j = 0; j < cuenta; j++) {
            waitpid(hijos[j], NULL, 0);
        }
        
        // Limpieza
        munmap(buffer, (N + 1) * sizeof(int));
        close(f);
        sem_close(llenas);
        sem_close(vacias);
        sem_close(mutex);
        
        // Eliminación de los semáforos
        sem_unlink("VACIAS");
        sem_unlink("LLENAS");
        sem_unlink("MUTEX");
        
        printf("\t\tTodos los consumidores acabaron\n");
    }
    
    return 0;
}

void consume_item() {
    if (buffer[N] < N - 1) { // Verifica si hay elementos para consumir
        buffer[N]++; // Incrementa primero para apuntar al último elemento insertado
    }

    if (buffer[N] < N) { // Si aún hay elementos
        int position = buffer[N];
        int consumed = buffer[position];
        int sum = consumed;
        
        for (int i = position + 1; i < N; i++) {
            sum += buffer[i];
        }
        
        printf("Condsumidor %d: Consumido el valor %d de la posicion %d, suma de valores en el buffer: %d\n", getpid(),consumed, position, sum);
    } else {
        printf("Buffer vacío, no se puede consumir.\n");
    }
}