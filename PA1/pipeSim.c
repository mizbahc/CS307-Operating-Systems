#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    printf("I’m SHELL process, with PID: %d - Main command is: man df | grep \"\\-a\" -A 1 > output.txt \n", (int)getpid());
    int fd[2];
    if (pipe(fd) < 0)
    {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    }

    pid_t rc[2];
    rc[0] = fork();

    if (rc[0] < 0)
    {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc[0] == 0)
    {
        printf("I’m MAN process, with PID: %d - My command is: man df\n", (int)getpid());

        rc[1] = fork(); // creating second child

        if (rc[1] < 0)
        {
            // fork failed; exit
            fprintf(stderr, "fork failed\n");
            exit(1);
        }
        else if (rc[1] == 0)
        {
            printf("I’m GREP process, with PID: %d - My command is: grep \"\\-a\" -A 1 > output.txt \n", (int)getpid());
            close(STDOUT_FILENO);
            open("./output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            close(fd[1]);

            close(STDIN_FILENO);
            dup(fd[0]); // exec process will use pipe as standart input

            char *myargs2[5];
            myargs2[0] = strdup("grep"); // program: "grep"
            myargs2[1] = strdup("\\-a"); // option -a, \ is escape charcter to disable special meaning of '-'
            myargs2[2] = strdup("-A");   // extra argument to read 1 line after finding -a
            myargs2[3] = strdup("1");    // number of lines to be read after -a
            myargs2[4] = NULL;           // marks end of array
            execvp(myargs2[0], myargs2); // runs man
        }
        else
        {
            close(fd[0]);         // Child-1 will not read from pipe
            close(STDOUT_FILENO); // closing stdout and dupping pipe output, process with exec will use pipe.
            dup(fd[1]);

            char *myargs[3];
            myargs[0] = strdup("man"); // program: "man"
            myargs[1] = strdup("df");  // command df called by man which reports file system disk space usage
            myargs[2] = NULL;          // marks end of array
            execvp(myargs[0], myargs); // runs man
        }
    }
    else
    {
        wait(NULL);
        printf("I’m SHELL process, with PID:%d - execution is completed, you can find the results in output.txt\n", (int)getpid());
    }

    return 0;
}