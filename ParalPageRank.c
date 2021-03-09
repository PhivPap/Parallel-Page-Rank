#include <stdio.h>
#include <stdlib.h>

#define INIT_GRAPH_NODES 1000

struct page_link{
    int page_id;
    struct page_link* next;
};

struct page{
    double rank;
    struct page_link* links; //list of links to other pages.
};

struct graph{ //singleton struct to hold all nodes (pages) of the graph
    struct page* array;
    size_t array_size;
    int pages_count;
};

struct graph pages;





void graph_init(void){
    pages.pages_count = 0;
    pages.array_size = INIT_GRAPH_NODES;
    pages.array = malloc(INIT_GRAPH_NODES * sizeof(struct graph));
}

void graph_double_nodes(void){
    pages.array_size *= 2;
    pages.array = realloc(pages.array, pages.array_size);
    if(!pages.array){
        print("Out of memory.\n");
        exit(137);
    }
}

void graph_add_page_link(int page_id, int page_link){
    while(page_id >= pages.array_size){ // if page_id oob: resize the array
        graph_double_nodes();
    }
    // do more....
}

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