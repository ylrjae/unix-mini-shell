#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_PIPES 12
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

int cmd_split(char *line, Command *command){
  int command_c = 0;
  char *token = strtok(line, "|");  

  while (token != NULL){
    parse_input(token, &command[command_c]);
    command_c ++;
    token = strtok(NULL, "|");
  }
  return command_c;
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
  
  Command commands[MAX_PIPES];
  int num_commands = cmd_split(line, commands);
  int pipes [MAX_PIPES][2];
  int num_pipes = num_commands -1;

  // Pipe implementation

  for (int i = 0; i < num_pipes; i++){

    if (pipe(pipes[i]) == -1){
        perror ("error during pipe process");
        exit(1);
      }
    } 

  if (commands[0].argc == 0){
    for( int i = 0; i < commands[0].argc; i++){
      free(commands[0].args[i]);
    }
    free(commands[0].args);
    continue;
  }

// cd implentation

if (strcmp(commands[0].args[0],"cd") == 0){
  if (commands[0].argc < 2) {
    chdir(getenv("HOME"));
  } else {
  if (chdir(commands[0].args[1]) != 0) {
      perror ( "cd failed");
    }
  }
continue;
}

// exit

if (strcmp(commands[0].args[0],"exit") == 0) {
  for ( int i = 0; i <commands[0].argc; i++){
      free(commands[0].args[i]);
  }
  free(commands[0].args);
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
      if (commands[0].outfile != NULL){

        int flag = commands[0].append
          ? O_WRONLY | O_CREAT | O_APPEND
          : O_WRONLY | O_CREAT | O_TRUNC;

        //fprintf(stderr, "DEBUG outfile: %s append: %d\n", commands[0].outfile, commands[0].append); 
        int fd = open(commands[0].outfile,flag, 0644);

        if (fd < 0) {              // catch error and exit
          perror ("file not found");
          exit(1);
        }
          dup2(fd,1);
          close(fd);
      }
        if (commands[0].infile != NULL){
        int fd = open(commands[0].infile, O_RDONLY);
        
        if (fd < 0){
          perror("file not found");
          exit(1);
        }
          dup2(fd,0);
          close(fd);
      }
    execvp(commands[0].args [0], commands[0].args);
    perror("exec failed");
    exit(1);
    }

    else {
      waitpid(pid, NULL, 0); 
    } 

  for( int i = 0; i < commands[0].argc; i++){
  free(commands[0].args[i]);
 }
  free(commands[0].args); // free parsed commands[0].args array before next loop iteration//

  }
 free(line); // free memory allocated to user input//

}    

