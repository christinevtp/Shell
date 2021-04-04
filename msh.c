/*
  Name: Christine Par
  ID: 1001490910
  Assignment 1
*/

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11

static void handle_signal (int sig )
{

}
int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  // declaring a char array called history that will be used to store the
  // commands that the user inputs, it will be able to list the last 15 commands
  char history[15][255];
  // integer history_index will be used as the counter for the first array of history
  int history_index = 0;

  // declaring an integer array to store each PID up until 15
  int pidArray[15];
  // declaring this counter for later use for listpids command
  int count = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    // this if statement checks the first character of cmd_str to see if its !
    if(cmd_str[0] == '!')
    {

      // if first char is !, the number entered right after! will be stored into
      // an integer called index
      int index = atoi(&cmd_str[1]);

      // then strncpy from history using the index from above into cmd_str
      // before cmd_str gets tokenized later
      strncpy(cmd_str, history[index-1], strlen(history[index]));

      // this if statement compares history_index - which is keeping count of
      // how many user inputs have been entered - to index - which is the value
      // that the user entered after !
      if (history_index < index)
      {
        // if the user inputs a number bigger than history_index, this message
        // will print out because the entered number command does not exist
        printf("Command not in history..\n");
      }
    }
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    // save a copy of the command line since strsep will end up
    // moving the pointer head
    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // string copy user input into our history array and increment our index
    strncpy(history[history_index++], cmd_str, 255);


    // this if statement will allow the cyclation of the 2D history array
    // when history_index is greater than 14, it will be set back to 0 to allow
    // strncpy to keep copying user input into the history array
    if (history_index > 14)
    {
      history_index = 0;
    }

    struct sigaction act;

    /*
      Zero out the sigaction struct
    */
    memset (&act, '\0', sizeof(act));

    /*
      Set the handler to use the function handle_signal()
    */
    act.sa_handler = &handle_signal;

    /*
      Install the handler and check the return value.
    */
    if (sigaction(SIGINT , &act, NULL) < 0) // SIGINT will stop the process
    {                                       // running in our shell
      perror ("sigaction: ");
      return 1;
    }
    if (sigaction(SIGTSTP, &act, NULL) < 0) // SIGTSTP will suspend the process
    {                                       // running in our shell
      perror("sigaction: ");
      return 1;
    }

    // this if statement will makesure that there is user input before hitting enter
    // if user hits enter without having any commands, it will prompt msh> again
    if (token[0] != NULL)
    {

      // this if statement compares the first command to both exit and quit
      // program will end if one of the two commands are entered
      if ((strcmp("exit", token[0]) == 0) || (strcmp(token[0], "quit") == 0))
      {
        exit(0);
      }
      else if ((strcmp("cd", token[0]) == 0))
      {
        // in this if statement, strcmp checks if user entered cd as the first input
        // if user did put cd, the functions chdir is called to change directory to
        // whatever the user typed after cd
        chdir(token[1]);
      }
      else if ((strcmp("history", token[0]) == 0))
      {
        // adding support for command to list the history of commands
        // if command entered == history, this for loop will list
        // the last 15 commands entered by the user thats IF the user has
        // entered 15 or more, if not, only the commands the user has entered
        // up to the point history was called will be listed
        int i = 0;
        for(i = 0; i < history_index; i++)
        {
          // the meaning and purpose of history_index and history have been
          // written ^^ higher up on this code
          printf("%d: %s", i+1, history[i]); // the i+1 will allow list number to
                                             // start from 1 instead of 0
        }

      }
      else if ((strcmp("listpids", token[0]) == 0))
      {
        // adding support for command to list pids of the processes spawned by shell
        // if command entered == listpids, this for loop will list the PIDS made by
        // this shell. It can list up to 15 PIDS, but only the PIDS that have
        // existed up to the point listpids was called will be listed
        int j = 0;
        for(j = 0; j < count; j++)
        {
          // pidArray and count were explained ^^ at the top of the code
          // how pids are stored in pidArray is explained below
          printf("%d: %d\n", j+1, pidArray[j]); // the j+1 will allow list number to
                                                // start from 1 instead of 0
        }
      }
      else
      {
        // fork a child called child_pid
        pid_t child_pid = fork();

        // here we store the PID of the child_pid in an array called pidArray
        // and incrementing count by 1 so that for every child thats forked,
        // the PID for that child will be stored into pidArray without writing
        // on top of the old PID
        pidArray[count++] = child_pid;
        int status;

        // in this if statement, if the process is a child then our exec family
        // member will take care of the command in two possible ways
        if( child_pid == 0 )
        {
          // execvp will use the path from the user account to search
          // it will execute whatever command is given by the user if it is
          // supported by execvp
          execvp(token[0], token);


          // if the command given is not supported, then two things can happen
          // in our case

          if (execvp(token[0], token) == -1)
          {
            if('!' == cmd_str[0])
            {
              // the first case is, if the first character passed by the user
              // is an exclaimation point '!', then it can be ignore in this loop,
              // because it was taken care of already ^^ way above in the code
              printf(" ");
            }
            else
            {
              // the second case is, the command doesn't exist therefore
              // we will pass the user this message and prompt msh> again
              printf("%s: Command not found\n\n",token[0]);
            }
          }

          // this if statement checks to see if the command option entered by
          // the user is a valid option
          if (execvp(token[1], token) == -1)
          {
            // if the command option entered by the user is invalid
            // perror will handle the error
            perror(token[1]);
          }
          exit( EXIT_SUCCESS );
        }
        // the parent is blocked until the next child process terminates
        wait(&status);
      }
    }
    // deallocate memory
    free( working_root );

  }
  return 0;
}
