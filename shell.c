#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


int main () {
    
    char *line = NULL;
    size_t len = 0;

    while (1){
	printf("myshell> "); //prints prompt//
	
	getline(&line, &len, stdin); //reads input//
	
	line [strcspn(line, "\n")] = 0;

	pid_t pid = fork(); //creates child processes//

	if (pid == 0) { 	//if process id = 0 send error message and exit //

	char  *args[] = {line, NULL};

	execvp(args[0], args);

	perror ("exec failed");
	exit(1);

	}

	else {				// if process id does not equal; execute//
	    waitpid(pid, NULL, 0);
	}

	printf(" Test: %s", line); //executes input//
    }
    
    free(line); // frees allocated memory//
}


