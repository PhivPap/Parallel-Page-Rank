#include <stdio.h>
#include <stdlib.h>


int main(int argc, const char* argv[]){

    int a, b;
    size_t size;
    char* str = NULL;
    while(getline(&str, &size, stdin) != -1){ // currently using stdin
        if(str[0] == '#') // skip comments.
            continue;
        sscanf(str, "%d\t%d", &a, &b);
        printf("%d\t%d\n", a, b);
    }


    if(!str)
        free(str);
}