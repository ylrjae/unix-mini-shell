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
  char *saveptr;
  char *token = strtok_r(line, "|", &saveptr);  

  while (token != NULL){
    parse_input(token, &command[command_c]);
    command_c++;
    token = strtok_r(NULL, "|", &saveptr);
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
  char *line_d = strdup(line);
  int num_commands = cmd_split(line_d, commands);
  free(line_d);

  int pipes [MAX_PIPES][2];
  int num_pipes = num_commands -1;

// input check
  if (commands[0].argc == 0){
    for( int i = 0; i < commands[0].argc; i++){
      free(commands[0].args[i]);
    }
    free(commands[0].args);
    continue;
  }


//cd implementation
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

  // Pipe implementation
  pid_t pid;
  pid_t pids[MAX_PIPES];

  for (int i = 0; i < num_pipes; i++) {
    if (pipe(pipes[i]) == -1){
        perror ("error during pipe process");
        exit(1);
    }
  } 

  // forking process

for (int i = 0; i < num_commands; i++){
    pid = fork();
    if (pid < 0) {
      perror ("fork failed");
      continue;
  } else if (pid ==0) {
    
     // wiring the pipe 
    if ( i < num_commands -1){ //check if not last command and use dup2 to set the pipe end and fd
        dup2(pipes[i][1],1);
      }
    if ( i > 0 ){              //check if not  first command and set dup 2 to set end of pipe and fd
        dup2(pipes[i-1][0],0);
      }
    
    // close pipe end
      for (int j = 0 ; j < num_pipes; j++){
        close(pipes[j][0]);
        close(pipes[j][1]);
      }
    // redirection process  
      if (commands[i].outfile != NULL){
      int flag = commands[i].append
        ? O_WRONLY | O_CREAT | O_APPEND
        : O_WRONLY | O_CREAT | O_TRUNC;
      int fd = open(commands[i].outfile,flag, 0644);
      if (fd < 0) {              // catch error and exit
         perror ("file not found");
         exit(1);
       }
         dup2(fd,1);
         close(fd);
       }
       if (commands[i].infile != NULL){
         int fd = open(commands[i].infile, O_RDONLY);
         if (fd < 0){
           perror("file not found");
           exit(1);
       }
       dup2(fd,0);
       close(fd);
      }
       execvp(commands[i].args[0], commands[i].args);
       perror("error creating process");
       exit(1);
     }
    pids[i] = pid;
}
// parent closes pipe ends
for ( int i = 0; i < num_pipes; i++) {
  close(pipes[i][0]);
  close(pipes[i][1]);
}

// waiting for children
for ( int i = 0; i < num_commands; i++) {
  waitpid(pids[i], NULL, 0);
}

// free commands
for (int i = 0; i < num_commands; i++){
  for( int j = 0; j < commands[i].argc; j++){
  free(commands[i].args[j]);
  }

  free(commands[i].args); // free parsed commands[0].args array before next loop iteration//

}

  } 
free(line); // free memory allocated to user input//
}   

