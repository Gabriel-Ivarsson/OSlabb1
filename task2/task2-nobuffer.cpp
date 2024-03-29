#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h> /* time */
#include <unistd.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

// No buffer

int main(int argc, char** argv) {
    struct shm_struct {
        int buffer;
        unsigned empty;
    };
    float randomSleep = 0;
    volatile struct shm_struct* shmp = NULL;
    char* addr = NULL;
    pid_t pid = -1;
    int var1 = 0, var2 = 0, shmid = -1;
    struct shmid_ds* shm_buf;
    time_t t;
    srand((unsigned)time(&t));

    /* allocate a chunk of shared memory */
    shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
    shmp = (struct shm_struct*)shmat(shmid, addr, 0);
    shmp->empty = 0;
    pid = fork();
    if (pid != 0) {
        /* here's the parent, acting as producer */
        while (var1 < 100) {
            /* write to shmem */
            var1++;
            while (shmp->empty == 1)
                ; /* busy wait until the buffer is empty */
            shmp->buffer = var1;
            shmp->empty = 1;
            printf("Sending %d\n", var1);
            fflush(stdout);
            randomSleep = rand() % 20 + 1;
            sleep((randomSleep / 10));
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    } else {
        /* here's the child, acting as consumer */
        while (var2 < 100) {
            /* read from shmem */
            while (shmp->empty == 0)
                ; /* busy wait until there is something */
            var2 = shmp->buffer;
            shmp->empty = 0;
            printf("Received %d\n", var2);
            fflush(stdout);
            randomSleep = rand() % 20 + 1;
            sleep((randomSleep / 10));
        }
        shmdt(addr);
        shmctl(shmid, IPC_RMID, shm_buf);
    }
}
