#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 64 //Defines a max of 64 slots to be used in the heap//
                    //pointers = 8 bytes. 64 * 8 = 512 bytes//

char **parse_input(char *line, int *argc){ // splits line into tokens and stores in array
                                           //returns the array of tokens and argc = token
                                           //count

  char **args = malloc(MAX_ARGS * sizeof(char*));
  *argc = 0;
  char *input = strdup(line); // duplicates line buffer so that it doesn't get destroyed
                              // by strtok's \0 bytes and can be re used
  char *token = strtok(input, " \t");

  while (token != NULL) {
      args[(*argc)++] = strdup(token);
      token = strtok(NULL, " \t");
    
}

args[*argc] = NULL;
free (input);
return args;
}

int main () {
    
    char *line = NULL;
    size_t len = 0;

    char **args;
    int argc;
    
    while (1){
	printf("$ > "); //prints prompt//
	
	if (getline(&line, &len, stdin) == -1) {  //reads input//
    break; 
}
	
	line [strcspn(line, "\n")] = 0;

  args = parse_input(line, &argc);

  if (argc == 0){
    for( int i = 0; i < argc; i++){
      free(args[i]);
    }
    free(args);
    continue;
  }

// cd implentation

if (strcmp(args[0],"cd") == 0){
  if (argc < 2) {
    chdir(getenv("HOME"));
  } else {
  if (chdir(args[1]) != 0) {
      perror ( "cd failed");
    }
  }
continue;
}

// exit

if (strcmp(args[0],"exit") == 0) {
  for ( int i = 0; i <argc; i++){
      free(args[i]);
  }
  free(args);
  free(line);
  exit(0);
}
// forking process 

	pid_t pid = fork(); //creates child processes//

  if (pid < 0) {
    perror("fork failed");
     continue;
  }
    else 	if (pid == 0) { 	//if process id = 0 send error message and exit //
    execvp(args [0], args);
    perror("exec failed");
    exit(1);
    }

    else {
      waitpid(pid, NULL, 0); 
    } 

  for( int i = 0; i < argc; i++){
  free(args[i]);
 }
  free(args); // free parsed args array before next loop iteration//

  }
 free(line); // free memory allocated to user input//

}    

