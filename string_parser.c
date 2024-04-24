/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 *  Editied on: April 8th, 2024
 *      Editor: Ellison Schilling/Largent
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token(char* buf, const char* delim) {
    
    // Checks if line is empty and returns 0 if so
    if (buf == NULL || strlen(buf) == 0) {
        return 0;
    }

    // Initialize a count for each token
    int token_count = 0;

    // Make a place to save the information
    char* saveptr = buf;

    // Make a place to save tokens temporarily
    char* token;

    // Go through buf until token is NULL adding to the token_count for each occurrence
    while ((token = strtok_r(saveptr, delim, &saveptr))) {

        // Skip empty tokens
        if (strlen(token) > 0) {
            token_count++;
        }
    }

    // Accounts for a string ending with a delimiter, but not a newline
    if ((strlen(buf) > 0 && buf[strlen(buf) - 1] == delim[0])|| buf[strlen(buf) - 1] != '\n') {
        token_count++;
    }

    return token_count;
}

command_line str_filler(char* buf, const char* delim) {
    // Creates a command_line variable to be filled and returned
    command_line cmd_line;
    cmd_line.command_list = NULL;
    cmd_line.num_token = 0;

    // Delete newline charcter if present
    if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    }

    // Make copy of the buffer to be sent to count_token
    char* temp_buf = malloc(strlen(buf) + 1);
    strcpy(temp_buf, buf);

    // Count the number of tokens with the count_token function
    cmd_line.num_token = count_token(temp_buf, delim);

    // Allocate memory for the token array inside the command_line variable based on the number of tokens
    cmd_line.command_list = (char**)malloc(sizeof(char*) * (cmd_line.num_token));

    // Find the tokens using strtok_r
    char* saveptr;
    char* token = strtok_r(buf, delim, &saveptr);
    int i = 0;
    while (token != NULL) {
        int tok_len = strlen(token);

        // Skip the last token if it's an empty string
        if (i == cmd_line.num_token - 1 && tok_len == 0) {
            break;
        }

        // Allocate memory for each token and copy it into the command_list
        cmd_line.command_list[i] = (char*)malloc(sizeof(char) * (tok_len + 1));
        
        // Handle memory allocation failure
        if (cmd_line.command_list == NULL) {
            return cmd_line;
        }

        // Insert token into command line list
        strcpy(cmd_line.command_list[i], token);

        // Remove the last newline character, if present
        if (tok_len > 0 && cmd_line.command_list[i][tok_len - 1] == '\n') {
            cmd_line.command_list[i][tok_len - 1] = '\0';
        }

        // Increment counter
        i++;

        // Iterate to next token in temporary buffer
        token = strtok_r(NULL, delim, &saveptr);
    }

    free(temp_buf);
    return cmd_line;
}

void free_command_line(command_line* command)
{
	// Go through the indencies and free each token occurance
	for (int i = 0; i < command->num_token; i++){
		free(command->command_list[i]);
	}
	
	// Free the empty command_list
	free(command->command_list);
}