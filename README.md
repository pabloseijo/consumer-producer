# Process Synchronization with Semaphores 🚦

This repository contains practical exercises for understanding and handling race conditions using semaphores, focusing on the producer-consumer problem 🔄. The exercises are developed in C and demonstrate the use of shared memory, semaphores, and process synchronization to manage critical sections.

## Exercise 1: Producer-Consumer Problem with Active Waiting 🏭

This exercise explores the producer-consumer problem using processes and active waiting to put processes to sleep and wake them up.

- **Objective**: To confirm the occurrence of race conditions when a producer writes to a buffer and a consumer reads from it using a "count" variable.
- **Buffer Size**: N=8, defined as a constant in the code.

### Compilation 🛠️

Navigate to the `ejercicio1` directory and compile the program with:
```
gcc -o productor ejercicio1/prodcutor.c 
gcc -o consumidor ejercicio1/consumidor.c 
```
### Execution 🚀

Run the program in two separate terminals with:

```
./productor
```

```
./consumidor
```

## Exercise 2: Producer-Consumer Solution with Semaphores 🔒

Based on Exercise 1, this exercise implements a solution using semaphores to manage critical sections without race conditions.

- **Details**:
  - The buffer is an `int` array functioning as a LIFO (Last In First Out) queue.
  - The `produce_item()` function generates a random integer between 0 and 10.
  - `insert_item(int item)` places the integer in the buffer.
  - `remove_item()` retrieves an integer from the buffer and sums it with all current values in the buffer.
  - `consume_item()` displays the read integer and the sum of the buffer's values.

### Compilation 🛠️

Navigate to the `ejercicio2` directory and compile the program with:
```
gcc -o productor2 ejercicio2/prodcutor2.c 
gcc -o consumidor2 ejercicio2/consumidor2.c 
```
### Execution 🚀

Run the program in two separate terminals with:

```
./productor2
```

```
./consumidor2
```

## Optional Exercises

### Exercise 3: Problem Solution with Threads (Optional) 🧵

This voluntary exercise solves the producer-consumer problem using threads instead of processes.

#### Compilation 🛠️

```
gcc -o threads_solution ejercicio3/threads_solution.c -pthread
```

#### Execution 🚀

```
./threads_solution
```

### Exercise 4: Generalized Process Solution (Optional) 📊

Generalizes the code for an arbitrary number of producers selected by the user.

#### Compilation 🛠️
```
gcc -o nProductores ejercicio2/nProductores.c 
gcc -o nConsumidores ejercicio2/nConsumidores.c 
```
### Execution 🚀

Run the program in two separate terminals with:

```
./nProductores
```

```
./nConsumidores
```

### Exercise 5: Dual LIFO Queue Exchange (Optional) ↔️

Implements another LIFO queue where the producer and consumer swap roles for the second queue.

#### Compilation 🛠️
```
gcc -o dual_queue ejercicio5/dual_queue.c -lpthread
```
#### Execution 🚀
```
./dual_queue
```
## Note 📝

All programs must be compiled with the `-pthread` option due to the use of POSIX threads and semaphores. Ensure that each program is executed within its respective directory to avoid path-related issues.
