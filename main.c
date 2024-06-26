/*
    Author: Ellison Schilling
    Date: 04/19/2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>
#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

void dealloc(char *buf, FILE *in, FILE *out) {
    /* Utility to free memory, and close files */
    if (buf != NULL)
        free(buf);
    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);
}

bool wrongNumArgs(int target_num, int token_num) {
    if (token_num != target_num) {
        printf("Error! Wrong number of arguments for command, please ensure proper formatting\n");
        return true;
    } else {
        return false;
    }
}

void parseCommand(command_line cmd_line) {
    if (cmd_list == NULL) {
        printf("Error! Invalid command_line structure\n");
        return;
    }
    char **cmd_list = cmd_line.command_list; // Get the list containing the command and args if applicable

    char *command = cmd_list[0]; // Get the command
    if (command == NULL) {
        printf("Error! Invalid command\n");
		return;
    }
    int num_tok = cmd_line.num_token; // Get the number of tokens

    // Process each command
    if (strcmp(command, "exit") == 0) {                // Exits on request
        free_command_line(&cmd_line);
        exit(0);
    } else if (strcmp(command, "ls") == 0) {           // Executes the ls command if possible
        if (!wrongNumArgs(2, num_tok)) {
            listDir();
        }
    } else if (strcmp(command, "pwd") == 0) {          // Executes the pwd command if possible
        if (!wrongNumArgs(2, num_tok)) {
            showCurrentDir();
        }
    } else if (strcmp(command, "mkdir") == 0) {        // Executes the mkdir command if possible
        if (!wrongNumArgs(3, num_tok)) {
            makeDir(cmd_list[1]);
        }
    } else if (strcmp(command, "cd") == 0) {           // Executes the cd command if possible
        if (!wrongNumArgs(3, num_tok)) {
            changeDir(cmd_list[1]);
        }
    } else if (strcmp(command, "cp") == 0) {           // Executes the cp command if possible
        if (!wrongNumArgs(4, num_tok)) {
            copyFile(cmd_list[1], cmd_list[2]);
        }
    } else if (strcmp(command, "mv") == 0) {           // Executes the mv command if possible
        if (!wrongNumArgs(4, num_tok)) {
            moveFile(cmd_list[1], cmd_list[2]);
        }
    } else if (strcmp(command, "rm") == 0) {           // Executes the rm command if possible
        if (!wrongNumArgs(3, num_tok)) {
            deleteFile(cmd_list[1]);
        }
    } else if (strcmp(command, "cat") == 0) {          // Executes the cat command if possible
        if (!wrongNumArgs(3, num_tok)) {
            displayFile(cmd_list[1]);
        }
    } else {
        printf("Error! Unrecognized command: %s \n", command);
        fflush(NULL);
    }
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);

    // Variables
    FILE *i_file;   // In file (holds the commands)
    FILE *o_file;   // Out file (receives the output)

    // Allocate memory for the buffer
    char *buffer;
    size_t bufSize = 32;
    buffer = (char *)malloc(bufSize * sizeof(char));

    // **** FILE MODE (ensures that there are three args and a file flag) ****
    if (argc == 3 && (strncmp(argv[1], "-f", 2) == 0 || strncmp(argv[1], "-file", 5) == 0)) {
        o_file = freopen("output.txt", "w", stdout);
        if (o_file == NULL) {
            printf("Error! Failed to open output file\n");
            dealloc(buffer, NULL, NULL);
            exit(1);
        }

        i_file = fopen(argv[2], "r");
        if (i_file == NULL) {
            printf("Error! Input file missing\n");
            dealloc(buffer, NULL, o_file);
            exit(1);
        }

        // declare line_buffer
        size_t len = 128;
        char *line_buf = malloc(len);
        command_line cmdList; // List of all the commands in a line

        // loop until the file is over
        while (getline(&line_buf, &len, i_file) != -1) {
            cmdList = str_filler(line_buf, ";"); 	// Deliniate the line by the ';' and sort into a list of commands
            for (int i = 0; cmdList.command_list[i] != NULL; i++) { 	// Go through each command
                if (cmdList.command_list[i] == NULL) {	// If command is NULL send error message
                    printf("Error! Invalid command_list element\n");
                    break;
                }
                command_line cmd;     // List of a command and its args (initialize each time to make sure no unititialized values)
				cmd = str_filler(cmdList.command_list[i], " ");	// Create a command_line for the command
				parseCommand(cmd);	// Execute command
                free_command_line(&cmd);	// Free command_line of the single command
            }
            free_command_line(&cmdList);	// Free command_list of commands
        }
        free(line_buf); // Free the dynamically allocated memory for line_buf
        printf("End of file \nBye Bye!");
        dealloc(buffer, i_file, o_file);
        return 0;
    } else {

        // **** INTERACTIVE MODE ****
        while (true) {
            printf(">>> ");
            command_line cmdList;
            char *input = NULL;
            size_t len = 0;
            ssize_t read;

            read = getline(&input, &len, stdin); // Get input from the command line
            if (read == -1) {
                printf("Error! Issue with reading input from console.\n");
                break;
            }

            // Remove newline character
            input[strcspn(input, "\n")] = '\0';

            if (strlen(input) == 0) {
                free(input);
                continue; // Empty input, prompt again
            }

            cmdList = str_filler(input, ";");
            for (int i = 0; cmdList.command_list[i] != NULL; i++) {
                if (cmdList.command_list[i] == NULL) {
                    printf("Error! Invalid command_list element\n");
                    continue;
                }
                command_line cmd;
				cmd = str_filler(cmdList.command_list[i], " "); // Parse the commands by the spaces to differentiate between the command and it's args
                parseCommand(cmd);	// Parse or call the command indicated by the commandline
                free_command_line(&cmd);	// Free this instance of the command
				memset(&cmd, 0, 0);

            }
            free_command_line(&cmdList);	// Free the entire list of commands
			memset(&cmdList, 0, 0);
            free(input);	// Free the given input
        }
        free(buffer); // Free the dynamically allocated buffer
    }
    return 0;
}