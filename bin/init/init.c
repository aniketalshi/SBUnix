#include <stdlib.h>

int main(int argc, char **argv)
{
    if (fork() == 0) {
        execvpe("bin/sh", NULL, NULL);
    }

    while(1);
    return 1;
}