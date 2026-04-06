#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_ARGS 64 //Defines a max of 64 slots to be used in the heap//
                    //pointers = 8 bytes. 64 * 8 = 512 bytes//

typedef struct {
  char   **args;
  int      argc;
  char  *infile;
  char *outfile;
  int    append;
} Command;

void parse_input(char *line, Command *cmd){ 

  cmd->argc = 0;
  cmd->args = malloc(MAX_ARGS * sizeof(char*));
  cmd->infile =  NULL;
  cmd->outfile = NULL;
  cmd->append = 0;

  char *input = strdup(line); // duplicates line buffer so that it doesn't get destroyed
                              // by strtok's \0 bytes and can be re used
  char *token = strtok(input, " \t");

  while (token != NULL) {
    if (strcmp(token, ">") == 0) {
      token = strtok(NULL, " \t");
      cmd -> outfile = strdup(token);
      cmd -> append = 0; 

    } else if (strcmp(token, ">>") == 0) {
      token = strtok(NULL, " \t");
      cmd -> outfile = strdup(token);
      cmd -> append = 1;

    } else if (strcmp(token, "<") == 0) {
      token = strtok(NULL, " \t"); 
      cmd->infile = strdup(token);

    } else {
      cmd->args [cmd->argc++] = strdup(token);
  }

token = strtok(NULL, " \t");

}

cmd->args[cmd->argc] = NULL;
free (input);

}

int main () {
    
    char *line = NULL;
    size_t len = 0;

    while (1){
	printf("$ > "); //prints prompt//
	
	if (getline(&line, &len, stdin) == -1) {  //reads input//
    break; 
}
	
	line [strcspn(line, "\n")] = 0;
  
  Command cmd;
  parse_input(line, &cmd);

  if (cmd.argc == 0){
    for( int i = 0; i < cmd.argc; i++){
      free(cmd.args[i]);
    }
    free(cmd.args);
    continue;
  }

// cd implentation

if (strcmp(cmd.args[0],"cd") == 0){
  if (cmd.argc < 2) {
    chdir(getenv("HOME"));
  } else {
  if (chdir(cmd.args[1]) != 0) {
      perror ( "cd failed");
    }
  }
continue;
}

// exit

if (strcmp(cmd.args[0],"exit") == 0) {
  for ( int i = 0; i <cmd.argc; i++){
      free(cmd.args[i]);
  }
  free(cmd.args);
  free(line);
  exit(0);
}
// forking process 

	pid_t pid = fork(); //creates child processes//

  if (pid < 0) {
    perror("fork failed");
     continue;
  }
    else 	if (pid == 0) { 	                    //if process id = 0 send error message and exit //
      if (cmd.outfile != NULL){

        int flag = cmd.append
          ? O_WRONLY | O_CREAT | O_APPEND
          : O_WRONLY | O_CREAT | O_TRUNC;

        //fprintf(stderr, "DEBUG outfile: %s append: %d\n", cmd.outfile, cmd.append); 
        int out = open(cmd.outfile,flag, 0644);

        if (out < 0) {              // catch error and exit
          perror ("file not found");
          exit(1);
        }
          dup2(out,1);
          close(out);
      }
        if (cmd.infile != NULL){
        int in = open(cmd.infile, O_RDONLY);
        
        if (in < 0){
          perror("file not found");
          exit(1);
        }
          dup2(in,0);
          close(in);
      }
    execvp(cmd.args [0], cmd.args);
    perror("exec failed");
    exit(1);
    }

    else {
      waitpid(pid, NULL, 0); 
    } 

  for( int i = 0; i < cmd.argc; i++){
  free(cmd.args[i]);
 }
  free(cmd.args); // free parsed cmd.args array before next loop iteration//

  }
 free(line); // free memory allocated to user input//

}    

