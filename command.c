/*******
    Author: Ellison Schilling
    Date: 4/19/2024
    Description: A file containing all possible commands for the psudo shell to execute  
*******/
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <stdbool.h>
# include "command.h"
# include <fcntl.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/stat.h>

/*
Description: My implementation of a print to standard out. Used to slim down the code, and make reading
easier. Helper function.
        Args: 
            char* string : The string we wish to print
        Returns: 
            N/A
*/
void myPrint(char* string){
	write(1, string, strlen(string));
}

/*
 Description: Checks if the provided string starts with the given prefix, a helper function.
        Args: 
            char *string : The string we wish to check against
			char *prefix : The prefix we want to know if is present
        Returns: 
            bool : true if the string starts with the prefix, false if not
*/
bool prefix(char *string, char *prefix) {
	if (strncmp(string, prefix, strlen(prefix)) == 0)
    	return true;
	else
		return false;
}

/*
    Description: From the current file list all other files in 
    the current directory. Operates as an ls command.
        Args: 
            N/A
        Returns: 
            N/A
*/
void listDir()
{
    char cur_dir[256]; // An empty string as big as it can go to hold the current directory path
    struct dirent *dirt;  // a pointer to a single directory entry
    DIR *dir = NULL; // a pointer for the directory stream
    char *file; // a pointer to an empty string that will be replaced by the file found in directories

    // Attempts to read the currecnt directory and get the path, prints error on failure
    if(getcwd(cur_dir, sizeof(cur_dir)) == NULL) {  
        myPrint("Error! unable to read from directory\n");
    }
    else {
        if((dir = opendir(cur_dir)) == NULL) {  // If the current pathway is not actually a directory 
                                                // pathway then an error message is printed
            myPrint("Error! unable to open directory\n");
        }
        else {
            while((dirt = readdir(dir)) != NULL) {  // While there is still more to read from the directory

                file = dirt->d_name;    // file is set to the name of the single given directory entry indicated by dirt
                strcat(file, " "); // Concatanates a space onto the filename to be printed
                myPrint(file); // Prints the given file
            }
            myPrint("\n"); // Print newline charcter for proper spacing
            closedir(dir); // Close the directory stream after iterating through it
        }
    }
}

/*
    Description: From the current file shows the pathway of the 
    current file. Operates as a pwd command.
        Args: 
            N/A
        Returns:    
            N/A
*/
void showCurrentDir()
{
    char cur_dir[256]; // An empty string as big as it can go to hold the current directory path

    // Attempts to read the currecnt directory and get the path, prints error on failure
    if(getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
        myPrint("Error! unable to read from directory\n");
    }
    else {
        strcat(cur_dir, "\n");  // Concataonates a new line charchter onto the path to be printed
        myPrint(cur_dir);   // Prints the current directory pathway
    }
}

/*
    Description: Create a new directory with the given name. Operates 
    as a mkdir command.
        Args: 
            char *dirName : The name of the directory the user wishes to create
        Returns: 
            N/A
*/
void makeDir(char *dirName)
{
    char cur_dir[64];   // Declare a character array of size 64 to store the current working directory
    char dir_path[256]; // Declare a character array of size 256 to store the path of the directory to be created

    getcwd(cur_dir, sizeof(cur_dir));   // Get the current working directory and store it in cur_dir

    strcpy(dir_path, "");   // Initialize dir_path with an empty string
    strcat(dir_path, cur_dir);  // Append the current working directory to dir_path
    strcat(dir_path, "/");  // Append a forward slash '/' to dir_path
    strcat(dir_path, dirName);  // Append the name of the directory to be created to dir_path

    if (mkdir(dir_path, 0666) != 0) {   // Call the mkdir function to create the directory
        myPrint("Error! could not create given directory\n");   // If mkdir fails, print an error message
    }
}

/*
    Description: Changes the current directory into the listed directory 
    if possible. Operates like the cd linux command.
        Args: 
            char *dirName : Name of the directory the user wishes to enter
        Returns:
            N/A
*/
void changeDir(char *dirName) {
    if (strcmp(dirName, "..") == 0) {
        // Handle the parent directory case
        char *parent_dir = (char *)malloc(256 * sizeof(char)); // Allocate memory for the parent directory path
        if (getcwd(parent_dir, 256) != NULL) {
            char *last_slash = strrchr(parent_dir, '/'); // Find the last '/' in the path
            if (last_slash != NULL) {
                *last_slash = '\0'; // Terminate the string at the last '/' to get the parent directory
                int changed_directory = chdir(parent_dir); // Change to the parent directory
                if (changed_directory != 0) {
                    myPrint("Error! Wasn't able to change the directory\n");
                }
            }
        }
        free(parent_dir); // Free the dynamically allocated memory for parent_dir
    } else if (strcmp(dirName, ".") == 0) {
        // Handle the current directory case
        // No need to do anything, as we're already in the current directory
    } else {
        // Handle the regular directory case
        int changed_directory = chdir(dirName); // Change to the specified directory
        if (changed_directory != 0) {
            myPrint("Error! Wasn't able to change the directory\n");
        }
    }
}

/*
    Description: Copies the file onto the destinationPath rewriting if needed
    any file at the preexisting path. Operates like the linux cp command.
        Args: 
            char *sourcePath : The path of the orignal file, used to copy
            char *destinationPath : The destination we wish to copy our file into
        Returns: 
            N/A
*/
void copyFile(char *sourcePath, char *destinationPath) { // cp
    int inFD, outFD; // Input and output file descriptors
    char buffer[256]; // Buffer to read and write data
    char *dest_file_name = NULL; // Pointer to store the destination file name

    inFD = open(sourcePath, O_RDONLY);  // Open source file
    if (inFD < 0) {
        myPrint("Error! Can't open input file\n");
        return;
    }

    // Get the file name from sourcePath
    dest_file_name = strrchr(sourcePath, '/');
    if (dest_file_name == NULL) // No '/' found, just use the source file name
        dest_file_name = sourcePath;
    else // Move one character ahead to avoid '/'
        dest_file_name++;

    // Change to the destination directory
    if (chdir(destinationPath) != 0) {
        myPrint("Error! Can't change directory to destination\n");
        close(inFD);
        return;
    }
    
    // Open or create the destination file and destination file descriptor, giving full permissions
    outFD = open(dest_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (outFD < 0) {    // Check if not successful
        myPrint("Error! Can't open destination file\n");
        close(inFD);    // Close input file
        return;
    } 

    // Write to output file what was written in the source file, the copying part of the function
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(inFD, buffer, sizeof(buffer))) > 0) { // While there is still more to read
        bytes_written = write(outFD, buffer, bytes_read);   // Write to output file
        if (bytes_written != bytes_read) {
            myPrint("Error! write error\n");
            close(inFD);    // Close files as needed on failure
            close(outFD);
            return;
        }
    }

    if (bytes_read < 0) { 
        myPrint("Error! read error\n");
        close(inFD);    // Close files as needed on failure
        close(outFD);
    }

    close(inFD);    // Close files
    close(outFD);
}

/*
    Description: Moves the file found at the source path to the location found via the
    destination path. Operates like the linux mv command.
        Args:
            char *sourcePath : The path of the orignal file, used to move
            char *destinationPath : The destination we wish to move our file into
        Returns:
            N/A
*/
void moveFile(char *sourcePath, char *destinationPath)
{   
    int file_moved = rename(sourcePath, destinationPath);     // Move the file, and check for success
    if(file_moved != 0) {
        myPrint("Error! can't move file\n");     // Give appropriate error on fail
    } 
}

/*
    Description: Deletes the given file. Operates like the linux rm command.
        Args: 
            char *filename : The name of the file the user wishes to delete
        Returns: 
            N/A
*/
void deleteFile(char *filename)
{
    int file_deleted = unlink(filename); // Call the unlink() system call to delete the file 
    if (file_deleted != 0) { // Check if the file deletion failed
    myPrint("Error! can't delete file\n"); // If it failed, print an error message
}
}

/*
    Description: Prints the contents of the provided file into the command line. 
    Operates like the linux cat command.
        Args: 
            char *filename : The name of the file the user wishes to see the 
            contents of
        Returns: 
            N/A
*/
void displayFile(char *filename) {
    int info, bytes_read;
    char *buf = (char*)malloc(1024 * sizeof(char)); // Allocate memory for a buffer of 1024 characters using calloc

    if (buf == NULL) {
        myPrint("Error! memory allocation failed\n");
        return;
    }
    info = open(filename, O_RDONLY); // Open the file specified by 'filename' in read-only mode
    if (info < 0) {
        myPrint("Error! can't open file\n"); // If the open() call fails, print an error message
        free(buf); // Free the dynamically allocated memory for the buffer
        return;
    }

    while ((bytes_read = read(info, buf, 1024)) > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {         // Print the contents of the buffer
            myPrint("Error! write error\n");
            close(info);
            free(buf);
            break;
        }
    }

    if (bytes_read < 0) {
        myPrint("Error! read error\n");
    }
    myPrint("\n");
    close(info); // Close the file
    free(buf); // Free the dynamically allocated memory for the buffer
}