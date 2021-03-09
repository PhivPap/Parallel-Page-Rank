#include <stdio.h>
#include <stdlib.h>

struct page_link{
    unsigned int page_id;
    struct page_link* next;
};

struct page{
    double rank;
    struct page_link* links; //list of links to other pages.
};

struct pages{ //singleton struct to hold all nodes (pages) of the graph
    struct page* array;
    unsigned int array_size;
    unsigned int pages_count;
};

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