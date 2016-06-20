#include <stdio.h> //printf
#include <unistd.h> //execl, execlp
#include <sys/types.h> // fork, getpid, getppid
#include <sys/wait.h> // wait
#include <stdlib.h> // exit
#include <fcntl.h>  // O_WRONLY|O_CREAT|O_TRUNC
#include <string.h> // strcpy


main(int argc, char* argv[]){

    const int STDIN = 0;
    const int STDOUT = 1;
    const int STDERR = 2;

    // we run the wrapper without arguments
    if (argc == 1) {

        //int fd[2], nbytes; // file descriptors for the pipe, nbytes which are read from the pipe
        int fd1;
        //char readbuffer[4096]; 
        int i,j;
        int status;
        int returned_status;
        int father;

        if((father = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if (father == 0)  // child
        {
            //да бъде посредник за играта (да я стартира и да изчаква нейното приключване):
            if ( execl("./tic-tac-toe.sh", "tic-tac-toe", NULL) == -1 ) {
                exit(99);
            }
        }

        wait(&status);
        returned_status = status / 256;

        //pipe(fd);
        // vzemame log failovete sortirani
        if((father = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if (father == 0)  // child
        {
            if( ( fd1 = open("temp", O_WRONLY|O_CREAT|O_TRUNC, 0600 ) ) == -1 ){
                    write(2, "Something went wrong!\n", sizeof("Something went wrong!\n")-1);
                    exit(99);
            }
            else {                
                    dup2(fd1, STDOUT);
                    execlp("ls", "ls", "./.tic-tac-toe", "-t", NULL);
            }          
        }
        
        wait(&status);
        returned_status = status / 256;

        // vzemame imeto na log faila
        char logFileName[64];


        //vzemame imeto na horata ot log faila
        char ime1[64];
        char ime2[64];


        int name = 0;
        if (( fd1=open("temp", O_RDONLY) ) == -1) {
            write(2, "Operation open failed!\n", sizeof("Operation open failed!\n")-1);
            exit(99);
        }

        i=0;
        int name2 = 0;
        char c[2] = " \0";
        while ( read(fd1, &c, 1) ) {
            // byte == int
            if (c[0] != '-' && name == 0) {
                ime1[i] = c[0];
                logFileName[i] = c[0];
            }
            else if (c[0] == '-' && name == 0) {
                ime1[i] = '\0';
                logFileName[i] = c[0];
                name = i;
            }
            else if (c[0] != '-' && name2 == 0) {
                ime2[i-name-1] = c[0];
                logFileName[i] = c[0];
            }
            else if (c[0] == '-' && name2 == 0 && c[0] != '\n' ) {
                ime2[i-name-1] = '\0';
                logFileName[i] = c[0];
                name2 = 1;
            }
            else if (c[0] != '-' && name2 > 0 && c[0] != '\n' ){
                logFileName[i] = c[0];
            }
            else { 
                logFileName[i] = '\0';
                break;
            }
            //printf("%s\n", c);
            i++;
        }

        close(fd1);

        printf("%s\n%s\n%s\n", ime1, ime2, logFileName);
        


        if ( father > 0 ){
            //father body
            // status returns the PID 
            
            
            printf("Status of execution of tic-tac-toe: %d\n", returned_status);
            if (returned_status == 99) {
                write(STDOUT, "Unsuccesful opening of file.\n", sizeof("Unsuccesful opening of file.\n")-1);
            } 
            else if (returned_status == 0) {
                write(STDOUT, "Game session ended.\n", sizeof("Game session ended.\n")-1);
            }
        } 
        else {
            //child body
            
        }
    }
}