#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include <fcntl.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <glob.h>
#include "arraylist.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef PRINTDEBUG
#define PRINTDEBUG 0
#endif

#ifndef PROCESSDEBUG
#define PROCESSDEBUG 0
#endif

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif

char *lineBuffer;
int linePos, lineSize;
int sizeOfArrayOfArrayLists;
int sizeForWildCard = 0;

list_t *formArgumentsLayeredArrayList(char [], int);
void printArrayOfArrayLists(list_t **, int);
void cleanUpArrayOfArrayLists(list_t **,int);
void processTheArrayOfArraylists(list_t **, int);
void pwdFunc(void);
void cdFunc(char*,int);
char* lookForBareName(char *);
char* checkIfProgramExists(char *);
char ** wildcardInterpreter(char *[], int);
void pipeFunc(list_t **);

int main (int argc, char **argv){
    //WILDCARD TESTING
    char *list[5];
    list[0] = "a";
    list[1] = "b";
    list[2] = "*.txt";
    list[3] = "c";
    list[4] = "d";

    char **resultList = wildcardInterpreter(list,5);
    printf("%s\n", resultList[0]);
    printf("%s\n", resultList[1]);
    printf("%s\n", resultList[2]);
    printf("%s\n", resultList[3]);
    printf("%s\n", resultList[4]);
    printf("%s\n", resultList[5]);
    printf("%s\n", resultList[6]);
    //printf("%s\n", resultList[7]);
    free(resultList[0]);
    free(resultList[1]);
    free(resultList[2]);
    free(resultList[3]);
    free(resultList[4]);
    free(resultList[5]);
    free(resultList[6]);

    free(resultList);





    //int size = 0;
    //for(int i = 0; resultList[i] != NULL; i++) {
      //  printf("%s\n",resultList[i]);
       // size++;
   // }
    //for(int i = 0; i < sizeForWildCard; i ++){
      //   free(resultList[i]);
     //}
     //free(resultList);



    // char *testList[] = resultList;

    // for(int i = 0; i < size; i++) {
    //     printf("%s\n", testList[i]);
    // }


    



    //END OF WILDCARD TESTING


    // if(argc > 1){
    //     if(DEBUG)printf("IN BATCH MODE\n"); 
    //     int bytes;
    //     char buffer[BUFSIZE];

    //     int fd = open(argv[1],O_RDONLY);
    //     int characterCounter = 0;

    //     while ((bytes = read(fd, buffer, BUFSIZE)) > 0){
    //         for(int i = 0; i < bytes; i ++){
    //             if(buffer[i] == '\n'){                   
    //                 char contentsOfTheFileCommand[characterCounter];
    //                 int indexer = 0;
    //                 for(int x = i - characterCounter; x < i; x ++){                      
    //                     contentsOfTheFileCommand[indexer] = buffer[x];
    //                     indexer ++;
    //                 }
    //                 for(int w = 0; w < characterCounter; w ++){
    //                     printf("%c\n",contentsOfTheFileCommand[w]);
    //                 }
    //             //printf("Created and running list\n");
    //             list_t *arrayOfArrayList = formArgumentsLayeredArrayList(contentsOfTheFileCommand, characterCounter);
                
    //             processTheArrayOfArraylists(&arrayOfArrayList,sizeOfArrayOfArrayLists);

    //             cleanUpArrayOfArrayLists(&arrayOfArrayList,sizeOfArrayOfArrayLists);
    //             characterCounter = 0;
    //             //printf("----------\n");
    //             }
    //             else{
    //                 characterCounter ++;
    //             }
    //         }
    //     }
    // }
    // else{
    //     if(DEBUG)printf("IN INTERACTIVE MODE\n");
    //     int bytes;
    //     char buffer[BUFSIZE];

    //     fputs("Welcome to my Shell!\n",stderr);

    //     while(1){
    //         fputs("mysh>",stderr);
    //         if((bytes = read(STDIN_FILENO, buffer, BUFSIZE)) > 0){ //read STDIN line into buffer
    //             char contentsOfSTDINArray[bytes];
    //             memcpy(contentsOfSTDINArray,buffer,bytes); //copy buffer into new array without trailing new line
                
    //             list_t *arrayOfArrayList = formArgumentsLayeredArrayList(contentsOfSTDINArray, bytes);
                
    //             if(PRINTDEBUG)printArrayOfArrayLists(&arrayOfArrayList,sizeOfArrayOfArrayLists);
                
    //             processTheArrayOfArraylists(&arrayOfArrayList,sizeOfArrayOfArrayLists);

    //             cleanUpArrayOfArrayLists(&arrayOfArrayList,sizeOfArrayOfArrayLists);
    //         }
    //     }
    // }
}

/*
Below function creates an array of arraylists
Each array index represents a subcommand
Each array index corresponds with an arraylist
The arraylist contains the arguments for the corresponding subcommand
*/
list_t * formArgumentsLayeredArrayList(char contentsOfSTDINArray[], int sizeOfSTDIN){
    contentsOfSTDINArray[sizeOfSTDIN-1] = '\0';
    //1 - get the amount of subcommands
    int sizeOfArray = 1;
    for(int i = 0; i < sizeOfSTDIN; i ++){
        if(contentsOfSTDINArray[i] == '|'){ //pipe has been found
            sizeOfArray ++; //amount of subcommands
        }
    }
    sizeOfArrayOfArrayLists = sizeOfArray;

    if(DEBUG)printf("Size of the array of arraylists is %d\n",sizeOfArray);

    //2 - process the arguments for each subcommand into arraylists
    list_t *arrayOfArrayLists = malloc(sizeOfArray*sizeof(list_t)); //we have the size, create the array
    for(int i = 0; i < sizeOfArray; i ++){
        list_t list;
        al_init(&list,1);
        memcpy(&arrayOfArrayLists[i],&list,sizeof(list));
    }
    int arrayOfArrayListsIndex = 0;

    if(DEBUG){printf("size of STDIN is %d\n",sizeOfSTDIN);}

    int characterCounter = 0; //counter for the size of the current string

    for(int i = 0; i < sizeOfSTDIN; i ++){
        if(DEBUG){printf("ArrayList index is %d\n",arrayOfArrayListsIndex);}
        if(DEBUG){printf("i is %d\n",i);}
        if(contentsOfSTDINArray[i] != ' ' && contentsOfSTDINArray[i] != '|' && i != (sizeOfSTDIN-1)){ //what we are looking at is a character
            characterCounter ++;
            if(DEBUG){printf("character count is %d\n",characterCounter);}
        }
        else if(contentsOfSTDINArray[i] == '|' && characterCounter == 0){ //we are currently at a pipe and there is no string beforehand
            arrayOfArrayListsIndex ++;
        }
        else if(((contentsOfSTDINArray[i] == ' ' || i == (sizeOfSTDIN-1) || contentsOfSTDINArray[i] == '|') && characterCounter != 0) || (sizeOfSTDIN == 1 && (contentsOfSTDINArray[i] != '|'))){ //we've reached whitespace but there is a set of characters beforehand
            if(DEBUG){printf("we've reached the end of a word!\n");}
            if(i == (sizeOfSTDIN-1))characterCounter ++;

            if(DEBUG){printf("character count at argument adding point: %d\n",characterCounter);}
            
            int startIndex; 
            if(i == (sizeOfSTDIN-1)){startIndex = i - (characterCounter-1);}
            else{startIndex = i - characterCounter;}

            char *strToBeAddedToArrayList = malloc(1024 * sizeof(char));
            strToBeAddedToArrayList[0] = '\0';

            if(DEBUG)printf("startIndex: %d\n",startIndex);
            
            //add the argument to a string
            if(i == (sizeOfSTDIN-1)){
                for(int x = startIndex; x <= i; x ++){
                    if(DEBUG)printf("x: %d\n",x);
                    strncat(strToBeAddedToArrayList, &contentsOfSTDINArray[x], 1);
                }
            }
            else{
                for(int x = startIndex; x < i; x ++){
                    if(DEBUG)printf("x: %d\n",x);
                    strncat(strToBeAddedToArrayList, &contentsOfSTDINArray[x], 1);
                }
            }

            if(DEBUG){printf("argument to be added is %s\n",strToBeAddedToArrayList);}

            //have the created argument string, NOW ADD TO THE ARRAY OF ARRAYLIST
            al_push(&arrayOfArrayLists[arrayOfArrayListsIndex], strToBeAddedToArrayList);

            if(contentsOfSTDINArray[i] == '|'){
                arrayOfArrayListsIndex ++;
                characterCounter = 0;
            }
            characterCounter = 0;
            //free(strToBeAddedToArrayList);
        }
    }
    return arrayOfArrayLists;
}

/*
Below function takes in the generated array of arraylist, and processes the subcommands given
The operation tokens are identified, and the specific operation is performed
*/
void processTheArrayOfArraylists(list_t **list,int sizeOfArrayOfArraylists){
    list_t *arrayOfArrayLists = *list;
    if(sizeOfArrayOfArrayLists == 1){ //there is no pipe in the subcommand
        for(int i = 0; i < al_length(&arrayOfArrayLists[0]); i ++){
            //CD implementation
            char *destCD;
            char *pathForCD;
            al_lookup(&destCD,arrayOfArrayLists,0);
            al_lookup(&pathForCD,arrayOfArrayLists,1);
            if(strcmp(destCD,"cd") == 0){
                if(al_length(&arrayOfArrayLists[0]) > 2){ //too many arguments
                    printf("mysh: cd: too many arguments\n");
                    return;
                }
                else if(al_length(&arrayOfArrayLists[0]) == 1){ //only cd has been entered
                    int chDirReturnCode = chdir(getenv("HOME"));
                    if(chDirReturnCode == -1){
                        printf("mysh: cd: invalid home path\n");
                        return;
                    }
                }
                else if(strcmp(destCD,"..") == 0 || strcmp(destCD,".") == 0 || strcmp(destCD,"-") == 0){ //trying to go one directory back
                    char *directory = malloc(1024 * sizeof(char));
                    directory[0] = '\0';
                    getcwd(directory,1024);
                    int characterCounter = 0;
                    for(int i = strlen(directory)-1; i >= 0; i --){
                        if(directory[i] != '/'){
                            characterCounter ++;
                        }
                        else{
                            characterCounter ++;
                            break;
                        }
                    } 
                    char *directoryUpdated = malloc(1024 * sizeof(char));
                    directoryUpdated[0] = '\0';
                    for(int i = 0; i <= strlen(directory)-characterCounter; i ++){
                        strncat(directoryUpdated,&directory[i],1);
                    }
                    chdir(directoryUpdated);
                    free(directory);
                    free(directoryUpdated);
                }
                else if(pathForCD[0] == '/'){ //we have a path
                    cdFunc(pathForCD,1);
                    return;
                }
                else if(pathForCD[0] == '~'){ //cd wildcard
                    int count = 0;
                    for(int j = 1; j < strlen(pathForCD); j ++){
                        count ++;
                    }
                    int envSize = strlen(getenv("HOME"));
                    int totalSize = envSize + count;

                    char *homePathPlusAdded = malloc(1024 * sizeof(char));
                    homePathPlusAdded[0] = '\0';

                    char *homePath = getenv("HOME");
                    int pathCounter = 1;

                    for(int w = 0; w < totalSize; w ++){
                        if(w < strlen(getenv("HOME"))){
                            strncat(homePathPlusAdded,&homePath[w],1);
                        }
                        else{
                            strncat(homePathPlusAdded,&pathForCD[pathCounter],1);
                            pathCounter ++;
                        }
                    }

                    cdFunc(homePathPlusAdded,1);
                    free(homePathPlusAdded);
                    return;
                }
                else{ //we have a bare name
                    cdFunc(pathForCD,0);
                    return;
                }
            }
            
            //PWD implementation
            char *destPWD;
            al_lookup(&destPWD,arrayOfArrayLists,0);
            if(strcmp(destPWD,"pwd") == 0){
                pwdFunc();
                return;
            }

            //< (input redirection) implementation
            char* inputRedirectionTokenChecker;
            al_lookup(&inputRedirectionTokenChecker, &arrayOfArrayLists[0], i);
            if(strcmp(inputRedirectionTokenChecker,"<") == 0){ //did the user enter <?
                if(PROCESSDEBUG)printf("WE'VE FOUND <\n");
                //generating string array of arguments
                int sizeOfArgumentStringArray = al_length(&arrayOfArrayLists[0])-1;
                if(PROCESSDEBUG)printf("size of the array is %d\n",sizeOfArgumentStringArray);
                char *argumentStringArray [sizeOfArgumentStringArray];
                int stringArrayIndex = 0;
                for(int i = 0; i < al_length(&arrayOfArrayLists[0]);i++){
                    char *he;
                    al_lookup(&he,&arrayOfArrayLists[0],i);
                    if(strcmp(he,"<") != 0){ //do not add if it is <
                        argumentStringArray[stringArrayIndex] = he;  
                        stringArrayIndex ++;
                    }
                }

                if(PROCESSDEBUG){printf("printing the string array of arguments last indices\n"); 
                    for(int i = 0; i < sizeOfArgumentStringArray; i ++){
                        printf("%s\n",argumentStringArray[i]);
                    }
                }

                //go through the array, check if every name/path exists
                for(int i = 0; i < sizeOfArgumentStringArray; i ++){
                    char *pathAtHand = checkIfProgramExists(argumentStringArray[i]);
                    if(strcmp(pathAtHand,"DNE") == 0){ //if the name/path does not exist, there is an error!
                       printf("mysh: <: fileName %s does not exist\n",argumentStringArray[i]);
                       return;
                    }
                    
                    //checking for text file, if a text file existsts => can't execute!
                    if(strlen(pathAtHand) >= 4){
                        if(pathAtHand[strlen(pathAtHand)-1] == 't' && pathAtHand[strlen(pathAtHand)-2] == 'x' && pathAtHand[strlen(pathAtHand)-3] == 't' && pathAtHand[strlen(pathAtHand)-4] == '.'){
                            printf("mysh: <: %s is a text file, cannot operate\n",pathAtHand);
                        }
                    }
                }

                //now that we have all the needed paths, set the first and last indices
                char *whatSTDINWillBeSetTo = argumentStringArray[sizeOfArgumentStringArray-1];
                argumentStringArray[sizeOfArgumentStringArray-1] = NULL;

                if(PROCESSDEBUG){
                    printf("Before fork() printing the argument list\n");
                    for(int i = 0; i < sizeOfArgumentStringArray-1; i ++){
                        printf("%s\n",argumentStringArray[i]);
                    }
                }

                //now that everything has been set, time to process!
                int pid = fork();
                if(pid == -1){
                    printf("mysh: <: child process cannot be created\n");
                    return; 
                }
                if(PROCESSDEBUG){printf("IN CENTER");}
                if(pid == 0){
                    //int fd = open(argumentStringArray[0],O_RDONLY);
                    if(PROCESSDEBUG){printf("%s\n", whatSTDINWillBeSetTo);}
                    int fd2 = open(whatSTDINWillBeSetTo, O_RDONLY);
                    if(PROCESSDEBUG){printf("%d\n", fd2);}
                    //set the final path to stdIN, now will execv with the executable and clean up
                    dup2(fd2, STDIN_FILENO);
                    if(PROCESSDEBUG){printf("%s\n", argumentStringArray[0]);}
                    execv(argumentStringArray[0], argumentStringArray); 
                    close(fd2);
                    //close(fd);
                }
                wait(&pid);
                return;
            }

            //> (output redirection) implementation
            char* outputRedirectionTokenChecker;
            al_lookup(&outputRedirectionTokenChecker, &arrayOfArrayLists[0], i);
            if(strcmp(outputRedirectionTokenChecker,">") == 0){ //did the user enter >?
                if(PROCESSDEBUG)printf("WE'VE FOUND >\n");

                int sizeOfArgumentStringArray = al_length(&arrayOfArrayLists[0])-1;
                if(PROCESSDEBUG)printf("size of the array is %d\n",sizeOfArgumentStringArray);
                char *argumentStringArray [sizeOfArgumentStringArray];
                int stringArrayIndex = 0;
                for(int i = 0; i < al_length(&arrayOfArrayLists[0]);i++){
                    char *he;
                    al_lookup(&he,&arrayOfArrayLists[0],i);
                    if(strcmp(he,">") != 0){ //do not add if it is <
                        argumentStringArray[stringArrayIndex] = he;  
                        stringArrayIndex ++;
                    }
                }

                if(PROCESSDEBUG){printf("printing the string array of arguments last indices\n"); 
                    for(int i = 0; i < sizeOfArgumentStringArray; i ++){
                        printf("%s\n",argumentStringArray[i]);
                    }
                }

                //GOING TO CALL WILDCARD INTERPRETER HERE BEFORE THE PATHS AND STUFF GET CREATED

                //go through the array, check if every name/path exists, if it does, truncate, if it does not, create
                for(int i = 0; i < sizeOfArgumentStringArray; i ++){
                    char *pathAtHand = checkIfProgramExists(argumentStringArray[i]);
                    if(strcmp(pathAtHand,"DNE") == 0){ //if the name/path does not exist, create
                       int fd = open(argumentStringArray[i],O_WRONLY | O_CREAT | O_TRUNC,0640);
                       close(fd);
                       char *generatedPath = checkIfProgramExists(argumentStringArray[i]);
                       argumentStringArray[i] = generatedPath;
                       continue;
                    }
                    //otherwise, truncate the file
                    int fd = open(pathAtHand,O_TRUNC | O_WRONLY);
                    close(fd);
                    argumentStringArray[i] = pathAtHand;
                }

                if(PROCESSDEBUG){printf("printing the string array of arguments after adding paths\n"); 
                    for(int i = 0; i < sizeOfArgumentStringArray; i ++){
                        printf("%s\n",argumentStringArray[i]);
                    }
                }

                //now that we have all the needed paths, set the first and last indices
                char *whatSTDINWillBeSetTo = argumentStringArray[sizeOfArgumentStringArray-1];
                argumentStringArray[sizeOfArgumentStringArray-1] = NULL;

                //now that everything has been set, time to process!
                int pid = fork();
                if(pid == -1){
                    printf("mysh: >: child process cannot be created\n");
                    return; 
                }
                if(pid == 0){
                    if(PROCESSDEBUG)printf("we in the child process for >\n");
                    int fd2 = open(whatSTDINWillBeSetTo, O_RDONLY);
                    //set the final path to stdIN, now will execv with the executable and clean up
                    dup2(fd2, STDOUT_FILENO);
                    execv(argumentStringArray[0], argumentStringArray); 
                    close(fd2);
                }
                wait(&pid);
                return;
            }
            
            //ls implementation
            if(strcmp(destCD,"ls") == 0){ //did the user enter ls?
                if(al_length(&arrayOfArrayLists[0]) == 1){ //there is only ls with no extra commands
                    if(PROCESSDEBUG)printf("IN LS EXECUTION\n");
                    int pid = fork();
                    if(PROCESSDEBUG)printf("pid in no argument return value is %d\n",pid);
                    if(pid == -1){
                        printf("the child could not be brought to life\n");
                        return;
                    }
                    if(pid == 0){
                        if(PROCESSDEBUG)printf("we are in the child process\n");
                        int fd = open("/usr/bin/ls",O_RDONLY);
                        char *directoryBareName = malloc(1024 * sizeof(char));
                        directoryBareName[0] = '\0';
                        getcwd(directoryBareName,1024);
                        execl("/usr/bin/ls",directoryBareName);
                        close(fd);               
                    }
                    wait(&pid);
                    return;
                }
                else{ //loop through the given commands and output the ls output for them
                    if(PROCESSDEBUG)printf("IN LS EXECUTION when output greater than one\n");
                    //create string array with exec as first argument and NULL as last argument
                    int sizeOfStringArrayOfArgs = (al_length(&arrayOfArrayLists[0])-1) + 2;
                    char *stringArrayOfArgs [sizeOfStringArrayOfArgs];
                    stringArrayOfArgs[0] = "/usr/bin/ls";
                    stringArrayOfArgs[sizeOfStringArrayOfArgs-1] = NULL;
                    if(PROCESSDEBUG)printf("size of this string array is %d\n",sizeOfStringArrayOfArgs);

                    //populate the string array
                    for(int i = 1; i < al_length(&arrayOfArrayLists[0]); i ++){
                        char *dest;
                        al_lookup(&dest,&arrayOfArrayLists[0],i);
                        char *pathForDest = checkIfProgramExists(dest);
                        if(strcmp(pathForDest,"DNE") == 0){ //if the argument has an invalid path
                            printf("mysh: ls: invalid path given\n");
                            return;
                        } 
                        stringArrayOfArgs[i] = pathForDest;
                    }

                    if(PROCESSDEBUG){
                        printf("printing the stringArrayOfArgs\n");
                        for(int i = 0; i < sizeOfStringArrayOfArgs-1; i ++){
                            printf("%s\n",stringArrayOfArgs[i]);
                        }
                    }

                    //have the string array, now execute ls
                    int pid = fork();
                    if(PROCESSDEBUG)printf("pid in more than one argument return value is %d\n",pid);
                    if(pid == -1){
                        printf("the child could not be brought to life\n");
                        return;
                    }
                    if(pid == 0){
                        int fd = open("/usr/bin/ls",O_RDONLY);
                        if(PROCESSDEBUG)printf("we are in the child process\n");
                        execv("/usr/bin/ls",stringArrayOfArgs);
                        close(fd);               
                    }
                    wait(&pid);
                    return;
                }
            }
            //echo implementation
            if(strcmp(destCD,"echo") == 0){ //has echo been entered?
                if(PROCESSDEBUG)printf("WE ARE IN ECHO\n");
                //populate the string array of arguments
                int sizeOfStringArrayOfArgs = (al_length(&arrayOfArrayLists[0])) + 1;
                char *stringArrayOfArgs [sizeOfStringArrayOfArgs];
                for(int i = 0; i < al_length(&arrayOfArrayLists[0]); i ++){
                    if(i == 0){
                        char *echoPath = checkIfProgramExists("echo");
                        stringArrayOfArgs[i] = echoPath;
                    }
                    char *dest;
                    al_lookup(&dest,&arrayOfArrayLists[0],i);
                    stringArrayOfArgs[i] = dest;
                }
                stringArrayOfArgs[sizeOfStringArrayOfArgs-1] = NULL;
                //now that we have the array of arguments, process!
                int pid = fork();
                if(PROCESSDEBUG)printf("pid in echo value is %d\n",pid);
                if(pid == -1){
                    printf("the child could not be brought to life\n");
                    return;
                }
                if(pid == 0){
                    char *echoPath = checkIfProgramExists("echo");
                    int fd = open(echoPath,O_RDONLY);
                    execv(echoPath,stringArrayOfArgs);
                    close(fd);
                }
                wait(&pid);
                return;
            }

            //cat implementation
            if(strcmp(destCD,"cat") == 0){
                if(PROCESSDEBUG)printf("WE ARE IN CAT\n");
                //populate the string array of arguments
                int sizeOfStringArrayOfArgs = (al_length(&arrayOfArrayLists[0])) + 1;
                char *stringArrayOfArgs [sizeOfStringArrayOfArgs];
                for(int i = 0; i < al_length(&arrayOfArrayLists[0]); i ++){
                    if(i == 0){
                        char *catPath = checkIfProgramExists("cat");
                        stringArrayOfArgs[i] = catPath;
                    }
                    char *dest;
                    al_lookup(&dest,&arrayOfArrayLists[0],i);
                    stringArrayOfArgs[i] = dest;
                }
                stringArrayOfArgs[sizeOfStringArrayOfArgs-1] = NULL;

                //now that we have the array of arguments, process!
                int pid = fork();
                if(PROCESSDEBUG)printf("pid in cat value is %d\n",pid);
                if(pid == -1){
                    printf("the child could not be brought to life\n");
                    return;
                }
                if(pid == 0){
                    char *catPath = checkIfProgramExists("cat");
                    int fd = open(catPath,O_RDONLY);
                    execv(catPath,stringArrayOfArgs);
                    close(fd);
                }
                wait(&pid);
                return;
            }
        }
    }
    else{ //there is one or more pipes in the subcommand
        if(PROCESSDEBUG)printf("WE ARE IN THE PIPES ELSE with arrayOfArrayList size %d\n",sizeOfArrayOfArrayLists);
        pipeFunc(&arrayOfArrayLists);
    }
}

/*
Below function processes the pipes of a command, interprets multiple pipes
*/
void pipeFunc(list_t **list){
    if(PROCESSDEBUG)printf("WE IN THE PIPEFUNC \n");
    //list_t *arrayOfArrayLists = *list;
    //initialize the list of pipes
    int **pipeList = malloc((sizeOfArrayOfArrayLists-1) * sizeof(int*));

    //go through the pipe list and initialize + call each pipe
    for(int i = 0; i < sizeOfArrayOfArrayLists-1; i ++){
        pipeList[i] = malloc(2 * sizeof(int));
        if(pipe(pipeList[i]) == -1){
            perror("pipe");
            exit(1);
        }
    }

    //go through the arrayOfArrayLists, set the pipes to STDIN or STDOUT, then execute the command
    int pipeListLooper = 0;
    for(int i = 0; i < sizeOfArrayOfArrayLists; i ++){
        int pid = fork();
        if(pid == 0){
            if(i == 0){ //currently at the beginning of the arrayOfArrayLists
                dup2(pipeList[pipeListLooper][1],STDOUT_FILENO);
            }
            else if(i == sizeOfArrayOfArrayLists-1){ //currently at the end of the arrayOfArrayLists
                dup2(pipeList[pipeListLooper][0],STDIN_FILENO);
            }
            else{ //currently in the middle of the arrayOfArrayLists
                dup2(pipeList[pipeListLooper][0],STDIN_FILENO);
                dup2(pipeList[pipeListLooper + 1][1],STDOUT_FILENO);
            }
            close(pipeList[pipeListLooper][0]);
            close(pipeList[pipeListLooper][1]);
            list_t *executeArrayOfArrayList = malloc(1*sizeof(list_t)); //we have the size, create the array 
            list_t list;
            al_init(&list,1);
            memcpy(&executeArrayOfArrayList[0],&list,sizeof(list));
            processTheArrayOfArraylists(&executeArrayOfArrayList,1);
            cleanUpArrayOfArrayLists(&executeArrayOfArrayList,1);
        }
        if(i % 2 == 1){
            close(pipeList[pipeListLooper][0]);
            close(pipeList[pipeListLooper][1]);
            pipeListLooper ++;
        }
        //wait(&pid);
    }

    int pidWait;
    for(int i = 0; i < sizeOfArrayOfArrayLists; i ++){
        wait(&pidWait);
    }

    for(int i = 0; i < sizeOfArrayOfArrayLists-1;i ++){
        free(pipeList[i]);
    }
    free(pipeList);
}

/*
Below function prints the working directory
*/
void pwdFunc(){
    char *directory = malloc(1024 * sizeof(char));
    directory[0] = '\0';
    getcwd(directory,1024);
    printf("%s\n",directory);
}

/*
Below function changes the current directory to file / path name specified (cd command)
Can go back one directory
Returns error message on invalid input
*/
void cdFunc(char *givenPath, int nameOrPath){
    //0 means bare name, 1 means path
    if(PROCESSDEBUG)printf("IN CDFUNC\n");
    if(PROCESSDEBUG)printf("we have path %s with nameOrPath %d\n",givenPath,nameOrPath);

    if(nameOrPath == 1){ //we have a path
        int chDirReturnCode = chdir(givenPath);
        if(chDirReturnCode == -1){
            printf("mysh: cd: %s: No such file or directory\n",givenPath);
        }
    }
    else if(nameOrPath == 0){ //we have a bare name
        char *directory = malloc(1024 * sizeof(char));
        directory[0] = '\0';
        getcwd(directory,1024);
        strncat(directory,"/",2);
        strncat(directory,givenPath,strlen(givenPath));
        struct stat cdStatBlock;
        int statReturnValue = stat(directory,&cdStatBlock);
        if(statReturnValue == 0){
            chdir(directory);
        }
        else{
            printf("mysh: cd: %s: No such file or directory\n",directory);
        }
        free(directory);
    }
}

/*
Below function checks if the given path or bare name exists
If such a path exists or the bare name exists in the working directory, the path is returned
If no path or file exists, then "DNE" is returned 
*/
char *checkIfProgramExists(char *pathOrBareName){
    if(pathOrBareName[0] == '/'){ //the given name/path is given in a path
        struct stat executablePathBlock;
        int executablePathBlockReturn = stat(pathOrBareName,&executablePathBlock);
        if(executablePathBlockReturn == -1){ //the path does not exist
            return "DNE";
        }
        else{ //the path does exist, just return it
            return pathOrBareName;
        } 
    }
    else{ //the given name/path is just a bareName
        char *directoryBareName = malloc(1024 * sizeof(char));
        directoryBareName[0] = '\0';
        getcwd(directoryBareName,1024);
        strncat(directoryBareName,"/",2);
        strncat(directoryBareName,pathOrBareName,strlen(pathOrBareName));
        struct stat directoryBareNameStruct;
        int directoryBareNameReturn = stat(directoryBareName,&directoryBareNameStruct);
        if(directoryBareNameReturn == 0){
            return directoryBareName;
        }
        else{
            char *lookForBareNameResult = lookForBareName(pathOrBareName);
            if(strcmp(lookForBareNameResult,"DNE") == 0){ //the given name does not exist
                return "DNE";
            }
            else{ //the given name does exist
                return lookForBareNameResult;
            }
        }
    }   
}

/*
Below function looks through the search path for given name
If not found, the operation will fal
If success, the found path is returned
*/
char *lookForBareName(char* name){
    if(PRINTDEBUG)printf("WE ARE IN lookForBareName\n");

    struct stat statBlock;

    char *path = malloc(1024 * sizeof(char));
    path[0] = '\0';
    char *path2 = malloc(1024 * sizeof(char));
    path2[0] = '\0';
    char *path3 = malloc(1024 * sizeof(char));
    path3[0] = '\0';
    char *path4 = malloc(1024 * sizeof(char));
    path4[0] = '\0';
    char *path5 = malloc(1024 * sizeof(char));
    path5[0] = '\0';
    char *path6 = malloc(1024 * sizeof(char));
    path6[0] = '\0';

    strncat(path,"/usr/local/sbin/",17);
    strncat(path,name,strlen(name));
    int statReturnValue = stat(path,&statBlock);
    if(statReturnValue == 0){
        if(PROCESSDEBUG)printf("FOUND THE FILE\n");
        return path;
    }
    else{
        free(path);
        strncat(path2,"/usr/local/bin/",16);
        strncat(path2,name,strlen(name));
        statReturnValue = stat(path2,&statBlock);
        if(statReturnValue == 0){
            if(PROCESSDEBUG)printf("FOUND THE FILE\n");
            return path2;
        }
    }
       
    free(path2);
    strncat(path3,"/usr/sbin/",11);
    strncat(path3,name,strlen(name));
    statReturnValue = stat(path3,&statBlock);
    if(statReturnValue == 0){
        if(PROCESSDEBUG)printf("FOUND THE FILE\n");
        return path3;
    }
    else{
        free(path3);
        strncat(path4,"/usr/bin/",10);
        strncat(path4,name,strlen(name));
        statReturnValue = stat(path4,&statBlock);
        if(statReturnValue == 0){
            if(PROCESSDEBUG)printf("YEEE\n");
            if(PROCESSDEBUG)printf("FOUND THE FILE\n");
            return path4;
        }
    }

    free(path4);
    strncat(path5,"/sbin/",7);
    strncat(path5,name,strlen(name));
    statReturnValue = stat(path5,&statBlock);
    if(statReturnValue == 0){
        if(PROCESSDEBUG)printf("FOUND THE FILE\n");
        return path5;
    }
    else{
        free(path5);
        strncat(path6,"/bin/",6);
        strncat(path6,name,strlen(name));
        statReturnValue = stat(path6,&statBlock);
        if(statReturnValue == 0){
            if(PROCESSDEBUG)printf("FOUND THE FILE\n");
            return path6;
        }
    }
    return "DNE";
}

char ** wildcardInterpreter(char *argumentStringArray[], int sizeOfArgumentStringArray){
    //printf("WE ARE IN THE WILDCARD FUNCTION\n");
    char **foundFile;
    glob_t gstruct;
    int globReturnValue;
    int sizeOfArrayThatWillBeReturned = sizeOfArgumentStringArray;

    for(int i = 0; i < sizeOfArgumentStringArray; i++){
        globReturnValue = glob(argumentStringArray[i], GLOB_ERR, NULL, &gstruct);

        if(globReturnValue == 0){
            sizeOfArrayThatWillBeReturned += (gstruct.gl_pathc-1);
        }
        globfree(&gstruct);
    }

    char** arrayThatWillBeReturned = malloc(sizeOfArrayThatWillBeReturned * sizeof(char*));
    int innerIndexer = 0;
    for(int i = 0; i < sizeOfArgumentStringArray; i++) {
        globReturnValue = glob(argumentStringArray[i], GLOB_ERR, NULL, &gstruct);
        if(globReturnValue != 0){ //no matches
            char *newStr = malloc((strlen(argumentStringArray[i]) + 1) * sizeof(char));
            //newStr[0] = '\0';
            strcpy(newStr,argumentStringArray[i]);
            //newStr = argumentStringArray[i];
            arrayThatWillBeReturned[innerIndexer] = newStr;
            //strcpy();
            //printf("%s\n", newStr);
            //strncpy(newStr, argumentStringArray[i], (strlen(argumentStringArray[i]) + 1));
            //strncpy(arrayThatWillBeReturned[innerIndexer], newStr, (strlen(newStr) + 1));
            //free(newStr);
            innerIndexer ++;
        }
        else{ //there is a match
            foundFile = gstruct.gl_pathv;
            while(*foundFile) {
                arrayThatWillBeReturned[innerIndexer] = malloc((strlen(*foundFile) + 1) * sizeof(char));
                //arrayThatWillBeReturned[0] = '\0';
                //arrayThatWillBeReturned[innerIndexer] = *foundFile;
                strcpy(arrayThatWillBeReturned[innerIndexer], *foundFile);
                //printf("%s\n", arrayThatWillBeReturned[innerIndexer]);
                //strncpy(arrayThatWillBeReturned[innerIndexer], *foundFile, (strlen(*foundFile) + 1));

                innerIndexer++;
                foundFile++;
            }
        }
        //printf("%s\n", arrayThatWillBeReturned[innerIndexer]);
        globfree(&gstruct);
    }

    //for(int i = 0; i < innerIndexer; i++) {
      //  printf("%s\n", arrayThatWillBeReturned[i]);
    //}
    sizeForWildCard = innerIndexer;
    return arrayThatWillBeReturned;
}

    /*
   //printf("total size is %d\n",sizeOfArrayThatWillBeReturned);

    //char *arrayThatWillBeReturned[sizeOfArrayThatWillBeReturned+1];
    char **arrayThatWillBeReturned = malloc(sizeOfArrayThatWillBeReturned * sizeof(char*));

    //arrayThatWillBeReturned[sizeOfArrayThatWillBeReturned] = NULL;
    int arrayThatWillBeReturnedIndexer = 0;
    int argumentStringArrayIndexer = 0;
    int amountOfMatchesThatHaveBeenAdded = 0;
    int matchWeAreOn = 0;
    int shifter = 0;

    for(int i = 0; i < sizeOfArrayThatWillBeReturned; i ++){
        arrayThatWillBeReturned[i] = malloc();
    }

     //for(int j = sizeOfArgumentStringArray; j < sizeOfArrayThatWillBeReturned; j ++){
       //  arrayThatWillBeReturned[j] = "test";
     //}

    //printf("WE HAVE INITIALIZED THE ARRAY\n");
    while(argumentStringArrayIndexer < sizeOfArgumentStringArray){
        //printf("ENTERED MAIN WHILE LOOP\n");
        globReturnValue = glob(argumentStringArray[argumentStringArrayIndexer], GLOB_ERR, NULL, &gstruct);
        
        if(globReturnValue != 0){
            printf("No matches found! Onto the next!\n");
        }
        else{
            amountOfMatchesThatHaveBeenAdded = gstruct.gl_pathc;
            printf("Found %d filename matches!\n", amountOfMatchesThatHaveBeenAdded);
            foundFile = gstruct.gl_pathv;
            while(*foundFile){
                char* fileToCheckMatching = malloc(strlen(*foundFile) * sizeof(char));
                //fileToCheckMatching[0] = '\0';
                //fileToCheckMatching[strlen(*foundFile)] = NULL;
                strncpy(fileToCheckMatching, *foundFile, strlen(*foundFile));
                printf("Indexer num: %d\n", arrayThatWillBeReturnedIndexer);
                if(amountOfMatchesThatHaveBeenAdded > 1){
                    if(matchWeAreOn == 0){//havent't added any match yet
                        //set at the index of the wildcard
                        arrayThatWillBeReturned[arrayThatWillBeReturnedIndexer] = fileToCheckMatching;
                        matchWeAreOn ++;
                    }
                    else{
                        //shift by one
                        for(shifter = sizeOfArrayThatWillBeReturned - 1; shifter >= arrayThatWillBeReturnedIndexer; shifter--){
                            arrayThatWillBeReturned[shifter] = arrayThatWillBeReturned[shifter - 1];
                        }
                        arrayThatWillBeReturned[arrayThatWillBeReturnedIndexer-1] = fileToCheckMatching;
                        printf("Array index %d will now get\n", arrayThatWillBeReturnedIndexer);
                    }
                }
                else{ //there's only one match
                    arrayThatWillBeReturned[arrayThatWillBeReturnedIndexer] = fileToCheckMatching;
                }    
                foundFile++;
            }
            matchWeAreOn = 0;
        }

        if(amountOfMatchesThatHaveBeenAdded != 0){
            arrayThatWillBeReturnedIndexer += amountOfMatchesThatHaveBeenAdded;
        }
        else{
            arrayThatWillBeReturnedIndexer++;
        }
        argumentStringArrayIndexer ++;
        globfree(&gstruct);
    }
    

    char** finalArray = arrayThatWillBeReturned;
    return finalArray;
}
*/

/*
Below function frees the elements of the array of arraylists containing subcommands and arguments
*/
void cleanUpArrayOfArrayLists(list_t **list, int size){
    list_t *arrayOfArrayListOutsideLoops = *list;
    for(int i = 0; i < size; i ++){
        al_destroy(&arrayOfArrayListOutsideLoops[i]);

    }
    free(arrayOfArrayListOutsideLoops);
}

/*
Below function prints out the array of arraylists containing subcommands and arguments
For debugging purposes
*/
void printArrayOfArrayLists(list_t ** arrayOfArrayLists, int size){
    printf("----------------------------\n");
    printf("PRINTING ARRAY OF ARRAYLISTS\n");
    list_t *arrayOfArrayListOutsideLoops = *arrayOfArrayLists;
    for(int i = 0; i < size; i++) {
        printf("ArrayList # %d with size %d\n", i,al_length(&arrayOfArrayListOutsideLoops[i]));
        for(int j = 0; j < al_length(&arrayOfArrayListOutsideLoops[i]); j++) {
            char* dest;
            int lookupReturn = al_lookup(&dest, &arrayOfArrayListOutsideLoops[i], j);
            printf("Element # %d : %s with return value %d\n", j, dest, lookupReturn);
        }
    }
    printf("----------------------------\n");
}