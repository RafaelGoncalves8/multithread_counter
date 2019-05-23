#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_WORKERS 4
#define MAX 100
#define ACTIVE 1
#define INACTIVE 0

typedef struct {
  int N;
  int ID;
} thread_args;

pthread_mutex_t lock;
int n_workers = 0;
pthread_t workers[MAX_WORKERS];
int worker_status[MAX_WORKERS];
int ans = 0; /* Quantidade de numeros primos. */

int is_prime(int n)
{
  int i = 5;
  if (n <= 3)
    return (n > 1); /* 0 0 1 */
  else if (n % 2 == 0 || n % 3 == 0)
    return 0;
  while (i * i <= n) /* Until i = sqrt(n). */
  {
    if (n % i == 0 || n % (i + 2) == 0) /* Divisible by 6k+-1. */
      return 0;
    i += 6;
  }
  return 1;
}

/* Funcao que cada thread executara, incrementa ans se for primo. */
void* worker(void *arg) {
  thread_args *info = (thread_args *)arg;
  pthread_mutex_lock(&lock);
  ans += is_prime(info->N);
  n_workers -= 1;
  worker_status[info->ID] = INACTIVE;
  pthread_mutex_unlock(&lock);
  free(info);
  return NULL;
}

int main() {
  thread_args *send_args;
  int nums[MAX];
  int i, j;
  char c;
  int n;

  i = 0;
  do {
    scanf("%d", &nums[i++]);
  }
  while((c = getchar()) != '\n');

  n = i;
  i = 0;

  while (i < n) {
      if (n_workers < MAX_WORKERS) {
          send_args = (thread_args*)malloc(sizeof(thread_args));
          send_args->N = nums[i++];
          j = 0;
          while (worker_status[j] == ACTIVE) j++;
          send_args->ID = j;
          pthread_mutex_lock(&lock);
          worker_status[j] = ACTIVE;
          n_workers += 1;
          pthread_mutex_unlock(&lock);
          pthread_create(& (workers[j]), NULL, worker, (void*) send_args);
         }
     }

  /* Esperando threads */
  for (int i=0; i<MAX_WORKERS; i++) {
    if (worker_status[i]==ACTIVE) {
      pthread_join(workers[i], NULL);
    }
  }
  printf("%d\n", ans);

  return 0;
}
