#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INIT_GRAPH_NODES 1000

struct page_link{
    int page_id;
    struct page_link* next;
};

struct page{
    bool is_used;
    double rank;
    struct page_link* links; // list of links to other pages.
};

struct graph{ // singleton struct to hold all nodes (pages) of the graph
    struct page* array;
    size_t array_size;
    int pages_count;
};

struct graph pages;





void graph_init(void){
    pages.pages_count = 0;
    pages.array_size = INIT_GRAPH_NODES;
    pages.array = malloc(INIT_GRAPH_NODES * sizeof(struct page));
    for(int idx = 0; idx < pages.array_size; idx++){
        pages.array[idx].links = NULL;
        pages.array[idx].is_used = false;
        pages.array[idx].rank = 1;
    }
}

void graph_free(void){
    // TODO
}

void graph_double_nodes(void){
    size_t prev_array_size = pages.array_size;
    pages.array_size *= 2;
    pages.array = realloc(pages.array, sizeof(struct page) * pages.array_size);
    if(!pages.array){
        printf("Out of memory.\n");
        exit(137);
    }
   
    for(int idx = prev_array_size; idx < pages.array_size; idx++){
        pages.array[idx].links = NULL;
        pages.array[idx].is_used = false;
        pages.array[idx].rank = 1;
    }
}

void graph_add_page_link(int page_id, int page_link){
    while(page_id >= pages.array_size){ // if page_id oob: resize the array;
        graph_double_nodes();
    }

    if(pages.array[page_id].is_used == false){
        pages.array[page_id].is_used = true;
        pages.pages_count++;
    }

    struct page_link* new_page_link = malloc(sizeof(struct page_link));
    new_page_link->page_id = page_link;
    new_page_link->next = pages.array[page_id].links;
    pages.array[page_id].links = new_page_link;
    
}

/* For debugging reasons */
void graph_print(){
    for(int idx = 0; idx < pages.array_size; idx++){
        if(!pages.array[idx].is_used)
            continue;
        printf("%d:\t", idx);
        struct page_link* ptr = pages.array[idx].links;
        while(ptr){
            printf("%d\t", ptr->page_id);
            ptr = ptr->next;
        }
        printf("\n");
    }
}

int main(int argc, const char* argv[]){
    int page_id, page_link;
    size_t size; // unused
    char* str = NULL;

    graph_init();
    
    while(getline(&str, &size, stdin) != -1){ // currently using stdin
        if(str[0] == '#') // skip comments.
            continue;
        sscanf(str, "%d\t%d", &page_id, &page_link);
        graph_add_page_link(page_id, page_link);
    }

    graph_print();
    if(!str)
        free(str);
    graph_free();
}