#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

int SEM_KEY = 230957;
int SHM_KEY = 129030;

void printerror(){
  printf("Error: %s\n", strerror(errno));
}

int main(){
  int semd = semget(SEM_KEY, 1, 0);
  int shmd = shmget(SHM_KEY, 0, 0);
  int * shmem = (int *)shmat(shmd, 0, 0);

  struct sembuf sbf[2];

  sbf[0].sem_num = 0;
  sbf[0].sem_op = -1;
  sbf[0].sem_flg = SEM_UNDO;

  semop(semd, sbf, 3);
  char last_line[2056];

  int filed = open("story.txt", O_RDONLY);
  //printf("Descriptor: %d\n", filed);
  lseek( filed, *shmem, SEEK_END );
  read(filed, last_line, sizeof(last_line));
  printerror();
  close(filed);

  //printf("last_line: %s\n", last_line);

  printf("Here's the last line of the story: %s \n", last_line);

  printf("Enter your line\n");

  char new_line[2056];

  fgets(new_line, sizeof(new_line), stdin);
  char * p = strchr(new_line, '\n');

  *shmem = strlen(new_line) * -1;

  //printf("Newline is: %s\n", new_line);
  //printf("shmem val: %d\n", *shmem);

  int filewd = open("story.txt", O_WRONLY | O_APPEND);
  //printf("Opened file for writing\n");
  int wri_work = write(filed, new_line, *shmem * -1);
  if(wri_work == -1) printerror();
  close(filed);

  //sbf[0].sem_op = 1;
  //semop(semd,sbf,3);
  shmdt(shmem);

}
