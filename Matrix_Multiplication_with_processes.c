#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<errno.h>
#include<string.h>

pid_t cur;
int rs[5], re[5], cs[5], ce[5];
int r1, r2, c1, c2;

struct Parsed{
	int num;
	char *file1, *file2;
};

void usage(){
	
	printf("Usage: Take parameters for matrix multiplication\n");
	printf("  Options:\n");
	printf("    -i  \tTakes filename path\n");
	printf("    -j  \tTakes filename path\n");
	printf("    -n  \tThe number of integers in a matrix is 2^n * 2^n\n");
exit(0);
	}

struct Parsed parseCommandLine(int argc, char *argv[]){
	int opt, count=0; 
	struct Parsed ret;
	int done[3] = {0,0,0};
	char *tmp;
	while((opt = getopt(argc, argv, ":i:j:n:")) != -1) 
	{ 
		switch(opt) 
		{ 
			case 'i': 
				if (done[0]!=0){
					printf("Invalid command line argument, i used twice\n");
					usage();			
				}
				ret.file1 = strdup(optarg);
				done[0] = 1;
				break;
			
			case 'j': 
				if (done[1]!=0){
					printf("Invalid command line argument, j used twice\n");
					usage();			
				}
				ret.file2 = strdup(optarg);
				done[1] = 1;
				break;

			case 'n': 
				if (done[2]!=0){
					printf("Invalid command line argument, n used twice\n");
					usage();			
				}
				done[2] = 1;
				count = strtol(optarg, &tmp, 10);
				if (*tmp  != '\0' || count>30 || count<0){
						printf("The option -n requires a number between 0 to 30 as an input\n"); usage();
				}
				ret.num = count;
				break;
			case ':': 
				printf("option needs a value\n");
				usage();
				break; 
			case '?': 
				printf("unknown option: %c\n", optopt); 
				usage();
		} 
	} 
	if(done[0]!=1 || done[1]!=1 || done[2]!=1){
		printf("Too few arguments\n");
		usage();
	}
	return ret;
};

void parent(int fd1[], int fd2[], int ind, int n, float Mat1[n][n], float Mat2[n][n], float Mat[n][n]){
//	printf("Parent %d\n", ind);	
	int count;
if (ind==1){
		close(fd1[0]);
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));

		for(int i=rs[1]; i<re[1]; i++) for(int j=cs[1]; j<ce[1]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[1]; i<re[1]; i++) for(int j=cs[1]; j<ce[1]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));

		for(int i=rs[2]; i<re[2]; i++) for(int j=cs[2]; j<ce[2]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[3]; i<re[3]; i++) for(int j=cs[3]; j<ce[3]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		close(fd2[1]); wait(NULL); // do something about it
		read(fd2[0], &count, sizeof(int));
		if(count==0) return;

		for(int i=rs[1]; i<re[1]; i++)
			for(int j=cs[1]; j<ce[1]; j++)
				read(fd2[0], &Mat[i][j], sizeof(float));
	}

else if (ind==2){
		close(fd1[0]);
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));

		for(int i=rs[1]; i<re[1]; i++) for(int j=cs[1]; j<ce[1]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[2]; i<re[2]; i++) for(int j=cs[2]; j<ce[2]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));

		for(int i=rs[2]; i<re[2]; i++) for(int j=cs[2]; j<ce[2]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[4]; i<re[4]; i++) for(int j=cs[4]; j<ce[4]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		close(fd2[1]); // do something about it
		read(fd2[0], &count, sizeof(int));
		if(count==0) return;

		for(int i=rs[2]; i<re[2]; i++)
			for(int j=cs[2]; j<ce[2]; j++)
				read(fd2[0], &Mat[i][j], sizeof(float));
	}

else if (ind==3){
		close(fd1[0]);
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));

		for(int i=rs[3]; i<re[3]; i++) for(int j=cs[3]; j<ce[3]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[1]; i<re[1]; i++) for(int j=cs[1]; j<ce[1]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));
	
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));
	
		for(int i=rs[4]; i<re[4]; i++) for(int j=cs[4]; j<ce[4]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[3]; i<re[3]; i++) for(int j=cs[3]; j<ce[3]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));
	
		close(fd2[1]); wait(NULL); // do something about it
		read(fd2[0], &count, sizeof(int));
		if(count==0) return;

		for(int i=rs[3]; i<re[3]; i++)
			for(int j=cs[3]; j<ce[3]; j++)
				read(fd2[0], &Mat[i][j], sizeof(float));
	}

else if (ind==4){

		close(fd1[0]);
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c1, sizeof(float));
		write(fd1[1], &r1, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));

		for(int i=rs[3]; i<re[3]; i++) for(int j=cs[3]; j<ce[3]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[2]; i<re[2]; i++) for(int j=cs[2]; j<ce[2]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		close(fd1[0]);
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));
		write(fd1[1], &r2, sizeof(float)); 	write(fd1[1], &c2, sizeof(float));

		for(int i=rs[4]; i<re[4]; i++) for(int j=cs[4]; j<ce[4]; j++)	write(fd1[1], &Mat1[i][j], sizeof(float));
		for(int i=rs[4]; i<re[4]; i++) for(int j=cs[4]; j<ce[4]; j++)	write(fd1[1], &Mat2[i][j], sizeof(float));

		close(fd2[1]); wait(NULL); // do something about it

		read(fd2[0], &count, sizeof(int));
		if(count==0) return;

		for(int i=rs[4]; i<re[4]; i++)
			for(int j=cs[4]; j<ce[4]; j++)
				read(fd2[0], &Mat[i][j], sizeof(float));
	}
}// parent

void child(int fd1[], int fd2[], int ind){
//	printf("child %d\n", ind);
//	sleep(1);
	int R1, C1, R2, C2;
	int count = 0;
	switch(ind){
		case 1: if(r1==0 || c1==0){
			close(fd2[1]); write(fd2[0], &count, sizeof(int)); exit(0);		
		}
		case 2: if(r1==0 || c2==0){
			close(fd2[1]); write(fd2[0], &count, sizeof(int)); exit(0);		
		}
		case 3: if(r2==0 || c1==0){
			close(fd2[1]); write(fd2[0], &count, sizeof(int)); exit(0);		
		}
		case 4: if(r2==0 || c2==0){
			close(fd2[1]); write(fd2[0], &count, sizeof(int)); exit(0);		
		}
	}


	close(fd1[1]);

	read(fd1[0], &R1, sizeof(float));	read(fd1[0], &C1, sizeof(float));
	read(fd1[0], &R2, sizeof(float));	read(fd1[0], &C2, sizeof(float));

// answer matrix	
	float Mat3[R1][C2]; for(int i=0;i<R1;i++) for(int c=0; c<C2; c++) Mat3[i][c] = 0.0;

	float Mat1[R1][C1], Mat2[R2][C2];

	for(int r=0; r<R1; r++) for(int c=0; c<C1; c++)	read(fd1[0], &Mat1[r][c], sizeof(float));
	for(int r=0; r<R2; r++)	for(int c=0; c<C2; c++)	read(fd1[0], &Mat2[r][c], sizeof(float));
/*
printf("\nMat1 seen\n");
for(int i=0; i<R1; i++) { for(int j=0; j<C1; j++) printf("%f ", Mat1[i][j]); printf("\n");}

printf("\nMat2 seen\n");
for(int i=0; i<R2; i++) { for(int j=0; j<C2; j++) printf("%f ", Mat2[i][j]); printf("\n");}
*/
	// matrix multiplication

	for(int i=0; i<R1; i++)
		for(int k=0; k<C2; k++)
			for(int j=0; j<C1; j++)
				Mat3[i][k] += Mat1[i][j] * Mat2[j][k];

/*
printf("\nMat3 seen\n");
for(int i=0; i<R1; i++){
	for(int j=0; j<C2; j++) printf("%f ", Mat3[i][j]); printf("\n") ;}
*/


	read(fd1[0], &R1, sizeof(float));	read(fd1[0], &C1, sizeof(float));
	read(fd1[0], &R2, sizeof(float));	read(fd1[0], &C2, sizeof(float));
	
	float Mat4[R1][C1], Mat5[R2][C2];

	for(int r=0; r<R1; r++) for(int c=0; c<C1; c++)	read(fd1[0], &Mat4[r][c], sizeof(float));
	for(int r=0; r<R2; r++)	for(int c=0; c<C2; c++)	read(fd1[0], &Mat5[r][c], sizeof(float));

/*
printf("\nMat4 seen\n");
for(int i=0; i<R1; i++) { for(int j=0; j<C1; j++) printf("%f ", Mat4[i][j]); printf("\n");}

printf("\nMat5 seen\n");
for(int i=0; i<R2; i++) { for(int j=0; j<C2; j++) printf("%f ", Mat5[i][j]); printf("\n");}
*/
	for(int i=0; i<R1; i++)
		for(int k=0; k<C2; k++)
			for(int j=0; j<C1; j++)
				Mat3[i][k] += Mat4[i][j] * Mat5[j][k];
	
close(fd2[0]);
count = R1*C2;
write(fd2[1], &count, sizeof(int));
/*
printf("\nMat3 seen\n");
for(int i=0; i<R1; i++){
	for(int j=0; j<C2; j++) printf("%f ", Mat3[i][j]); printf("\n");}
printf("------------------\n");
*/

	for(int i=0; i<R1; i++) for(int j=0; j<C2; j++)	write(fd2[1], &Mat3[i][j], sizeof(float));
	exit(0);
}

int setpipe(int n, int m, int fd1[n][2], int fd2[n][2]){
// m is the number of pipes
	for(int i=1; i<=m;i++){
		if ( pipe(fd1[i-1])==-1 ){
			perror("Pipe Failed for fd1");
			return 1;	
		}

		if ( pipe(fd2[i-1])==-1 ){
			perror("pipe Failed for fd2");
			return 1;	
		}
	}

	rs[1] = 0; re[1] = n/2; cs[1] = 0, ce[1] = n/2;
	rs[2] = 0; re[2] = n/2; cs[2] = n/2, ce[2] = n;
	rs[3] = n/2; re[3] = n; cs[3] = 0, ce[3] = n/2;
	rs[4] = n/2; re[4] = n; cs[4] = n/2, ce[4] = n;
	r1 = n/2; r2=n-n/2;
	c1 = n/2; c2=n-n/2;

	return 0;
}

void readfile(char * filename, int times, float arr[]){
	FILE * fp; fp = fopen(filename, "r");
	char tmp[200]; strcpy(tmp, filename);
	int i=0;
	
	if( fp == 0 )
    {
        switch(errno)
        {
            case EPERM:
                perror("Operation not permitted");
                break;
            case ENOENT:
            case ENFILE:
				strcat(tmp, " not found");
                perror(tmp);
                break;
            case EACCES:
                perror(filename);
                break;
            case ENAMETOOLONG:
                perror("Filename is too long");
                break;
            default:
                fprintf(stderr,"Unknown error\n");
        }
        exit(0);
    }

while(i<times  && !feof(fp))
	fscanf(fp, "%f", &arr[i++]);

//printf("i = %d, times= %d\n", i, times);
if(i < times) { printf("Not enough values in %s \n", filename); exit(0);} 
	fclose(fp);

}

void get_val(float arr1[], float arr2[], int size, struct Parsed values){
	readfile(values.file1, size , arr1);
	readfile(values.file2, size , arr2);
	//for(int i=0; i<size*size; i++)
	//	scanf("%f", &arr2[i]);
}

void handler(int signum){
	kill(cur, SIGTERM);
	printf("\nSignal Ctrl + C Caught\n");
	printf("Killing open child processes\n");
	printf("bye\n");

	exit(signum);
}

int main(int argc, char *argv[]){
	signal(SIGINT, handler);
	
	struct Parsed values = parseCommandLine(argc, argv);
	int PROC = 4, fd1[PROC][2], fd2[PROC][2], N=1;
	pid_t pid;
	for(int i=0; i<values.num; i++) N *= 2; // N is 2^n
	
	float arr1[N*N], arr2[N*N];
	get_val(arr1, arr2, N*N, values);
	
	float MAT1[N][N], MAT2[N][N], MAT3[N][N];
	for(int i=0; i<N; i++)
		for(int j=0 ;j<N; j++)
			MAT1[i][j] = arr1[i*N+j];

	for(int i=0; i<N; i++)
		for(int j=0 ;j<N; j++)
			MAT2[i][j] = arr2[i*N+j];


	if(setpipe(N, 4, fd1, fd2)) exit(0);
	
// -----------------------------------------------

	// process 1
	pid = fork();
	if (pid < 0 ){
		perror("forking failed for process 1"); exit(0);
	}
	else if (pid==0){
		// child process
		child(fd1[0], fd2[0], 1);
	}
	else if (pid > 0) {
		cur = pid;
		parent(fd1[0], fd2[0], 1, N, MAT1, MAT2, MAT3);
		signal(SIGCHLD, SIG_IGN);
		pid = fork();
	}

// process 2
	if (pid < 0 ){
		perror("forking failed for process 2"); exit(0);
	}
	else if (pid==0){
		// child process
		child(fd1[1], fd2[1], 2);
	}

	else if (pid > 0) {
		cur = pid;
		parent(fd1[1], fd2[1], 2, N, MAT1, MAT2, MAT3);
		signal(SIGCHLD, SIG_IGN);
		pid = fork();
	}

	// process 3
	if (pid < 0 ){
		perror("forking failed for process 3"); exit(0);
	}
	else if (pid==0){
		// child process
		child(fd1[2], fd2[2], 3);
	}

	else if (pid > 0) {
		cur = pid;
		parent(fd1[2], fd2[2], 3, N, MAT1, MAT2, MAT3);
		signal(SIGCHLD, SIG_IGN);
		pid = fork();
	}

	// process 4
	if (pid < 0 ){
		perror("forking failed for process 4"); exit(0);
	}
	else if (pid==0){
		// child process
		child(fd1[3], fd2[3], 4);
	}

	else if (pid > 0) {
		cur = pid;
		parent(fd1[3], fd2[3], 4, N, MAT1, MAT2, MAT3);
		signal(SIGCHLD, SIG_IGN);
	}

// ----------------------------------------
/*	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++) printf("%f ", MAT1[i][j]);
		printf("\n");
	}

printf("\n\n");
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++) printf("%f ", MAT2[i][j]);
		printf("\n");
	}
printf("\n");
*/

	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++) printf("%-20f ", MAT3[i][j]);
		printf("\n");
	}
return 0;
}
