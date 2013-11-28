#include <stdlib.h>
#include <defs.h>
#include <sys/dirent.h>

char currdir[1024];
char temp[512];
DIR *tp;

int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return (0);

    return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}

int ustrlen(const char *str)
{
    int len=0;
    while (*str++ != '\0')
        len += 1;
    return len;
}

char* strcat(char *str1, const char *str2)
{
    uint64_t len1 = ustrlen(str1);
    uint64_t len2 = ustrlen(str2);
    uint64_t i = 0;

    for(i = 0; i < len2 ; i++)
        str1[len1 + i] = str2[i];
    str1[len1 + i] = '\0';

    return str1;    
}

char *strcpy(char *dest, const char *src)
{
    char *str = dest;
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
    return str;
}

char *getLine(char *ptr, char *str)
{
    while (*ptr != '\n')
        *str++ = *ptr++;
    *str = '\0';

    return ++ptr;
}

int argsCount(char *str)
{
    int num=0;

    while (*str != '\0')
    {
        if( *str == ' ')
            num++;

        str++;
    }
    return num;
}

void modify_string(char *currdir)
{
    int i=0,j=0,k=0;

    for(i = 0 ; currdir[i] != '\0' ; )
    {

        if (currdir[i] != '.')
        {
            temp[j++] = currdir[i++];

        }
        else if(currdir[i] == '.' && currdir[i+1] == '.')
        {
            i+=3;
            j-=2;
            for(k = j; temp[k] != '/'; k--)
            {

            }
            j=k+1;
        }
    }
    temp[j]='\0';
    
    strcpy(currdir, temp);
}


int main(int argc, char **argv)
{
    //buffer is to hold the commands that the user will type in
    char /*str[25], *newstr,*/ ptr[20]; //= "Hello World Program\nls -l\nls -a\n\0";
    int i, j=0, k=0;//, count;
    // /bin/program_name is the arguments to pass to execv
    //if we want to run ls, "/bin/ls" is required to be passed to execv()
    char *path = "bin/";
    tp = opendir("/");
    strcpy(currdir, "/"); 
    //volatile int exit_now = 0;
    while(1)
    {
        char args[20][20];

        printf("\n"); 
        printf("[user@SBUnix ~%s]$", currdir);
        scanf("%s", ptr);
        //newstr = ptr;
        j=0;
        k=0;
        //count = argsCount(ptr); 


        for (i = 0; i < ustrlen(ptr); i++)
        {
            if(ptr[i] == ' ') {
                args[j][k]= '\0';
                j++;
                k=0;
            } else 
                args[j][k++] = ptr[i];


        }
        *ptr = NULL;

        args[j][k]='\0';


        //for(i = 0; i <= j; i++);
        //printf("\t%s",args[i]);
        if (strcmp(args[0], "pwd") == 0) {
            printf("\n%s", currdir); 

        } else if (strcmp(args[0], "cd") == 0) {

            int lendir  = ustrlen(currdir);

            strcat(currdir, "/");
            strcat(currdir, args[1]);


            tp = opendir(currdir); 

            if(tp == NULL) {
                printf("\n Invalid path entered"); 
                currdir[lendir] = '\0';
            }

            modify_string(currdir);

        } else if (strcmp(args[0], "ls") == 0) {
            printf("\n");
            struct dirent* temp; 
            while((temp = readdir(tp)) != NULL) {
                printf("\t%s", temp->name);
            }

        } else {

            char prog[20];
            strcpy(prog, path);
            //printf("\nprog:%s", prog);
            strcat(prog, args[0]);
            //printf("\nfinal prog:%s", prog);
            //int l;
            //printf("\t%s\t%d", str, argsCount(str));

            //printf("\tnumber of args:%d \targs:", count);
            //for (l=1; l<= count; l++)
            //printf("\t%s", args[l]);

            //fork and execv calls start 
            //if (prog[4] == 'e') {
            //printf("\n");
            //exit(1); 
            //exit_now = 1;
            //exit(1);
            //} else {
            //fork!
            int pid = fork();
            //Error checking to see if fork works
            //If pid !=0 then it's the parent
            if(pid!=0)
            {
                wait(NULL);
            }
            else
            {
                execvpe(prog, NULL, NULL);
                exit(1);
            }

            // fork and execv ends
            //}
        } 
    }

    /*
       newstr = ptr;
    //For parsing a script file and extracting the commands from the file
    while (*newstr != '\0')
    {
    char args[20][20];
    int count;
    newstr = getLine(newstr, str);
    j=0;
    k=0;
    count = argsCount(str);
    for (i = 0; i < ustrlen(str); i++)
    {
    if(str[i] == ' ') {
    args[j][k]= '\0';
    j++;
    k=0;
    } else 
    args[j][k++] = str[i];


    }
     *str = NULL;

     args[j][k]='\0';


     for(i = 0; i <= j; i++);
    //printf("\t%s",args[i]);


    char prog[20];
    strcpy(prog, path);
    printf("\nprog:%s", prog);
    strcat(prog, args[0]);
    printf("\nfinal prog:%s", prog);
    int l;
    printf("\t%s\t%d", str, argsCount(str));
    printf("\targs:");
    for (l=1; l< count; l++)
    printf("\t\t%s", args[l]);

    }
    */ 
    /*
       while(1)
       {
    //print the prompt
    printf("<shell>");
    //get input
    read(stdin, buffer, 512);
    //fork!
    //int pid = fork();
    //Error checking to see if fork works
    //If pid !=0 then it's the parent
    if(pid!=0)
    {
    wait(NULL);
    }
    else
    {
    //if pid = 0 then we're at teh child
    //Count the number of arguments
    int num_of_args = argCount(buffer);
    //create an array of pointers for the arguments to be passed to execcv.
    char *arguments[num_of_args+1];
    //parse the input and arguments will have all the arguments to be passed to the program
    parseArgs(buffer, num_of_args, arguments);
    //set the last pointer in the array to NULL. Requirement of execv
    arguments[num_of_args] = NULL;
    //This will be the final path to the program that we will pass to execv
    char prog[512];
    //First we copy a /bin/ to prog
    strcpy(prog, path);
    //Then we concancate the program name to /bin/
    //If the program name is ls, then it'll be /bin/ls
    strcat(prog, arguments[0]);
    //pass the prepared arguments to execv and we're done!
    int rv = execv(prog, arguments);
    }
    }
    */
    //    while(1);
    return 0;
}
