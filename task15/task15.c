/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 4

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
        {
            /* Simple initialization, which enables us to easy check
	         * the correct answer. Each element in c will have the same
	         * value as SIZE after the matmul operation.
	         */
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
}

static void
matmul_seq()
{
    int i, j, k;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
            printf(" %7.2f", c[i][j]);
        printf("\n");
    }
}

void mulRow(int row)
{
    for (int i = 0; i < SIZE; i++)
    {
        c[row][i] = 0.0;
        for (int k = 0; k < SIZE; k++)
        {
            pthread_mutex_lock(&lock);
            c[row][i] = c[row][i] + a[row][k] * b[k][i];
            pthread_mutex_unlock(&lock);
        }
        
    }
}

void *child(void *id)
{
    int row = (int)id;
    printf("My row is %d\n", row);
    fflush(stdout);
    mulRow(row);
    return NULL;
}

int main(int argc, char **argv)
{
    init_matrix();

    pthread_t *children;
    int id = 0;

    children = malloc(SIZE * sizeof(pthread_t));
    for (id = 0; id < SIZE; id++)
    {
        fflush(stdout);
        pthread_create(&(children[id]), NULL, child, (void *)id);
    }
    for (id = 0; id < SIZE; id++)
    {
        pthread_join(children[id], NULL);
    }
    free(children);
    print_matrix();
}