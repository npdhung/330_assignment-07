/*
 *  Hung Nguyen
 *  Z1924897
 *  CSCI330 - 002
 *
 *  Assignment 7
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {

    // if number of arguments given is incorrect, prompt the user
    if ((argc < 3)||((argc == 3)&&!strcmp(argv[1],"-c"))) {
        cout << "Usage: seclog [-c] out_file message_string" << endl;
        cout << "   where the message_string is appended to file out_file." << endl;
        cout << "   The -c option clears the file before file message is appended" << endl;
        exit(EXIT_FAILURE);
    }

    bool cOption;
    char* fileName;
    fileName = new char;
    char* message;
    message = new char;
    cOption = !strcmp(argv[1], "-c");
    if (cOption) {
        strcpy(fileName, argv[2]);
        strcpy(message, argv[3]);
    } else {
        strcpy(fileName, argv[1]);
        strcpy(message, argv[2]);
    }
    strcat(message,"\n"); // add new line character

    // check permission of current directory
    int readDir;
    struct stat statDir;
    readDir = stat(".", &statDir);
    (void)readDir;
    if (!(S_IWUSR & statDir.st_mode)) {
        cout << "Permission denied." << endl;
        exit(EXIT_FAILURE);
    }
    
    int readStatus;
    struct stat statBuffer;
    // call stat system call
    readStatus = stat(fileName, &statBuffer);
    // check if a file is exist, if not create a new one
    int fileDescriptor = 0;
    bool newFileOption = false;
    int rs;
    if (readStatus != 0) {
        fileDescriptor = creat(fileName,O_WRONLY);
        newFileOption = true;
        rs = chmod(fileName, 00000);
        if (rs == -1) {
            perror(fileName);
            exit(EXIT_FAILURE);
        }
    }
    
    // call stat system call
    readStatus = stat(fileName, &statBuffer);
    if (readStatus == -1) {
        perror(fileName);
        exit(EXIT_FAILURE);
    }

    // check whether filename is a file
    if (!S_ISREG(statBuffer.st_mode)) {
        cout << fileName << " is not a file." << endl;
        exit(EXIT_FAILURE);
    }
    
    // check permission
    bool permFlag = true; // return true if NO permission is exist

    if (S_IRUSR & statBuffer.st_mode) permFlag = false;
    if (S_IWUSR & statBuffer.st_mode) permFlag = false;
    if (S_IXUSR & statBuffer.st_mode) permFlag = false;
    if (S_IRGRP & statBuffer.st_mode) permFlag = false;
    if (S_IWGRP & statBuffer.st_mode) permFlag = false;
    if (S_IXGRP & statBuffer.st_mode) permFlag = false;
    if (S_IROTH & statBuffer.st_mode) permFlag = false;
    if (S_IWOTH & statBuffer.st_mode) permFlag = false;
    if (S_IXOTH & statBuffer.st_mode) permFlag = false;
    
    if (!permFlag) {
        cout << fileName << " is not secure. Ignoring." << endl;
        exit(EXIT_FAILURE);
    }
    
    //change permission to allow user to write to file
    rs = chmod(fileName, 00200);
    if (rs == -1) {
        perror(fileName);
        exit(EXIT_FAILURE);
    }
    readStatus = stat(fileName, &statBuffer);
    
    ssize_t count;
    
    // If there is -c option, the program will overwrite content
    // if not, it will append to the exist content
    if (!newFileOption) {
        if (cOption) fileDescriptor = open(fileName, O_WRONLY | O_TRUNC);
        else fileDescriptor = open(fileName, O_WRONLY | O_APPEND);
    }
    if (fileDescriptor == -1) {
        perror(fileName);
        exit(EXIT_FAILURE);
    }

    // write to file
    count = write(fileDescriptor, message, strlen(message));
    if (count == -1) {
        perror(fileName);
        exit(EXIT_FAILURE);
    }

    // close file
    close(fileDescriptor);

    //change back to NO permission
    rs = chmod(fileName, 00000);
    
    return 0;
}

