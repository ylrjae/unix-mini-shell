# myshell

    This project is a basic unix shell built to understand system and UNIX programming in C.
## Features

-Command executions with arguments.

    For example: ls -la, cd /tmp

-Input redirection utilizing "<", ">" and ">>".

    The shell can handle reading and writing operations including appending

    Example: ls > files.txt, cat < files.txt. 

-Piping

     The shell can pipe different commands via the use of "|". 
    
     Example: ls | grep .c 

-Built-ins

     The shell can use basic unix commands such as cd and exit.

-Signal handling

     The shell can kill a process (parent/child) by using CTRL-C


## To build the project we can use "make run" or just use the ./myshell binary

## What I learned doing this project

    This project helped me get and understanding about  how memory management,pipes, forking, parent/child relationship between them
    work, signal handling and how strings and tokenizing work in C. It also helped me understand UNIX system programming basics
    by learning how to utilize file descriptors, flags, and reading and writing work using stdout/stdin and other unix functions
    to process and handle data.
