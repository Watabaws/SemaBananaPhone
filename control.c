#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int SEM_KEY = 230957;
int SHM_KEY = 129030;

int main(int argc, char * argv[]){

	if(!strcmp(argv[1], "-c")){
		int semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0600);
		int shmd = shmget(SHM_KEY, 256, IPC_CREAT | IPC_EXCL | 0600);

		if(semd == -1){
			printf("Story already exists\n");
		}
		else{
			union semun {
				int              val;
				struct semid_ds *buf;
				unsigned short  *array;
				struct seminfo  *__buf;
			} arg;
			arg.val = 1;
			semctl(semd, 0, SETVAL, arg);
			int shmd = shmget(SHM_KEY, 0, 0);
			int * shmem = (int *)shmat(shmd, 0, 0);
			*shmem = 0;
			printf("Shared memory created.\n");
			printf("Semaphore created, value set to 1\n");
			int filed = open("story.txt", O_CREAT | O_TRUNC, 0755);
			printf("File created.\n");
			close(filed);
		}
	}

	else if(!strcmp(argv[1], "-v")){
		int filed = open("story.txt", O_RDONLY, 0755);
		char story[256];
		read(filed, story, sizeof(story));
		printf("Here's your story:\n%s\n", story);
		close(filed);
	}

	else if(!strcmp(argv[1], "-r")){
		int semd = semget(SEM_KEY, 1, 0);
		int shmd = shmget(SHM_KEY, 0, 0);
		int filed = open("story.txt", O_RDONLY, 0755);
		char story[256];

		struct sembuf sbf[2];

	  sbf[0].sem_num = 0;
	  sbf[0].sem_op = -1;
	  sbf[0].sem_flg = SEM_UNDO;

	  semop(semd, sbf, 3);
		read(filed, story , sizeof(story));
		semctl(semd, 0, IPC_RMID);
		printf("Semaphore released.\n");
		shmctl(shmd, IPC_RMID, 0);
		printf("Shared memory released. \n");
		printf("Here's the final story: \n%s\n", story);
		remove("story.txt");
		close(filed);

	}
}
