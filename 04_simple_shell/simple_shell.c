#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 100 /* The maximum length command */
#define COMMAND_LEN MAX_LEN / 2 + 1

int check_background(char *args)
{
    int check = 0;

    for (int i = 0; i < strlen(args); i++) {
        if (strcmp(args[i], "&") == 0)
            check = 1;
    }

    return check;
}

int main(void)
{
    char *args[COMMAND_LEN]; /* command line arguments */
    char *input= (char*) malloc(MAX_LEN * sizeof(char));
    char *cmd_ptr;
    int pid, status, retval;
    int check = 0;
    int cmd_count = 0;
    int should_run = 1; /* flag to determine when to exit program */
    int background = 0;

    while (should_run) {
        printf("my_shell> ");
        fflush(stdout);
        /**
         * 표준입출력으로부터 문자열을 입력 받은 후:
         * (1) fork()를 통해 자식 프로세스를 생성
         * (2) 자식 프로세스가 execvp()를 호출하도록 할 것
         * (3) 만약 입력받은 문자에 &가 포함되어 있으면,
         *     부모 프로세스는 wait() 호출
         */
        // Input command
        fgets(input, MAX_LEN, stdin);

        if (strcmp(input, "\n") == 0) {
            continue;
        }

        // Setting args
        cmd_ptr = strtok(input, " \n");
        while (cmd_ptr != NULL) {
            args[cmd_count] = cmd_ptr;

            cmd_ptr = strtok(NULL, " \n");
            cmd_count ++;
        }
        args[cmd_count] = NULL;

        // Handling commands
        if (strcmp(args[0], "exit") == 0) {
            exit(1);
        }

        // Check background
        if (strcmp(args[cmd_count - 1], "&") == 0) {
            args[cmd_count - 1] = NULL;
            check = 1;
        }

        // Make child
        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "[ERROR] Fork failed\n");
            
            exit(1);
        } else if (pid == 0) {
            printf("[INFO] In child process ...\n");
            execvp(args[0], args);

            printf("[DEBUG] Not showing this\n");
            exit(1);
        } else {
            printf("[INFO] In parent process ...\n");

            // If has not background cmd (&)
            if (check == 0) {
                retval = waitpid(pid, &status, 0);
                printf("[INFO] Run with NOT background process (Waiting the child process ...)\n");
                printf("[INFO] Child process exited ... (status: %d)\n", status);

                if (retval > 0) {
                    if (WIFEXITED(status)) {
                        printf("[INFO] Child process exited normally : %d\n", WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("[INFO] Child process exited by signal : %d\n", WTERMSIG(status));
                    } else {
                        printf("[ERROR] Child process exited by error\n");
                    }
                }
            } else {
                printf("[INFO] Run with background process\n");
            }
        }

        cmd_count = 0;
        check = 0;
    }

    free(input);

    return 0;
}
