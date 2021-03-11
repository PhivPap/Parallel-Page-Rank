#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define REVOLUTIONS 50
#define INIT_GRAPH_NODES 1000

typedef long long int int64;

struct Page_link{
    int64 page_id;
    struct Page_link* next;
};

struct Page{
    bool is_used;
    double rank;
    struct Page_link* links; // list of links to other pages.
    int links_count;
};

struct Graph{ // singleton struct to hold all nodes (pages) of the graph
    struct Page* array;
    size_t array_size;
    int64 pages_count;
    int64 last_page_idx;
};

struct thread_arg{
    int64 start_idx, end_idx;
    int revolutions;
};

struct Graph pages;





void graph_init(void){
    pages.last_page_idx = -1;
    pages.pages_count = 0;
    pages.array_size = INIT_GRAPH_NODES;
    pages.array = malloc(INIT_GRAPH_NODES * sizeof(struct Page));
    for(int64 idx = 0; idx < pages.array_size; idx++){
        pages.array[idx].links = NULL;
        pages.array[idx].is_used = false;
        pages.array[idx].rank = 1;
        pages.array[idx].links_count = 0;

    }
}

void graph_free(void){
    for(int64 idx = 0; idx < pages.array_size; idx++){
        if(!pages.array[idx].is_used)
            continue;
        struct Page_link* ptr = pages.array[idx].links;
        struct Page_link* prev_ptr;
        while(ptr){
            prev_ptr = ptr;
            ptr = ptr->next;
            free(prev_ptr);
        }
    }
    free(pages.array);
}

void graph_double_nodes(void){
    size_t prev_array_size = pages.array_size;
    pages.array_size *= 2;
    pages.array = realloc(pages.array, sizeof(struct Page) * pages.array_size);
    if(!pages.array){
        printf("Out of memory.\n");
        exit(137);
    }
   
    for(int64 idx = prev_array_size; idx < pages.array_size; idx++){
        pages.array[idx].links = NULL;
        pages.array[idx].is_used = false;
        pages.array[idx].rank = 1;
        pages.array[idx].links_count = 0;
    }
}

void graph_add_page_link(int64 page_id, int64 page_link){
    if(page_id > pages.last_page_idx)
        pages.last_page_idx = page_id;
    if(page_link > pages.last_page_idx)
        pages.last_page_idx = page_link;


    while(page_id >= pages.array_size){ // if page_id oob: resize the array;
        graph_double_nodes();
    }

    if(pages.array[page_id].is_used == false){
        pages.array[page_id].is_used = true;
        pages.pages_count++;
    }

    struct Page_link* new_page_link = malloc(sizeof(struct Page_link));
    new_page_link->page_id = page_link;
    new_page_link->next = pages.array[page_id].links;
    pages.array[page_id].links = new_page_link;
    pages.array[page_id].links_count++;
}

/* For debugging reasons */
void graph_print(){
    for(int64 idx = 0; idx < pages.array_size; idx++){
        if(!pages.array[idx].is_used)
            continue;
        printf("%lld:\t", idx);
        struct Page_link* ptr = pages.array[idx].links;
        while(ptr){
            printf("%lld\t", ptr->page_id);
            ptr = ptr->next;
        }
        printf("\n");
    }
}

void graph_print_ranks(){
    printf("node,pagerank\n");
    for(int64 idx = 0; idx < pages.last_page_idx; idx++){
        printf("%lld,%f\n", idx, pages.array[idx].rank);
    }
}

void page_rank_single_thread(){
    int64 pages_visited = 0;
    int64 idx = 0;
    while(pages_visited < pages.pages_count){
        if(!pages.array[idx].is_used){
            idx++;
            continue;
        }
        struct Page_link* ptr = pages.array[idx].links;
        // increases rank of all neighboring pages. 
        // (pages that the particular page has links to)
        double normalized_rank = pages.array[idx].rank / (double)pages.array[idx].links_count;
        while(ptr){ 
            pages.array[ptr->page_id].rank += 0.85 * normalized_rank;
            ptr = ptr->next;
        }  

        pages_visited++;
        idx++;
    }
}

void* page_rank_multi_thread(void* _arg){
    struct thread_arg* arg = (struct thread_arg*)_arg;
//     struct thread_arg{
//     int64 start_idx, end_idx;
//     int revolutions;
// };
}

/* 
    argv[1] = <count of threads> 
*/
int main(int argc, const char* argv[]){

    int thread_count = 1;
    int64 page_id, page_link;
    size_t size;
    char* str = NULL;

    if(argc > 1){
        thread_count = atoi(argv[1]);
        if(thread_count <= 0 || thread_count > 4){
            printf("Set valid thread-count.\n");
            return 1;
        }
    }

    graph_init();
    
    // graph parsing -------------------------------------------------------
    while(getline(&str, &size, stdin) != -1){ // currently using stdin
        if(str[0] == '#') // skip comments.
            continue;
        sscanf(str, "%lld %lld", &page_id, &page_link);
        graph_add_page_link(page_id, page_link);
    }

    // thread generating ---------------------------------------------------
    int64 pages_per_thread = pages.pages_count / thread_count;
    int64 page_idx = 0;
    for(int i = 0; i < thread_count - 1; i++){
        struct thread_arg* thread_info = malloc(sizeof(struct thread_arg));
        thread_info->start_idx = page_idx;
        thread_info->revolutions = REVOLUTIONS;
        int64 thread_page_count = 0;
        while(thread_page_count < pages_per_thread){
            if(pages.array[page_idx].is_used)
                thread_page_count++;
            page_idx++;
        }
        thread_info->end_idx = page_idx - 1;
        // create thread here with arg.
    }
    
    struct thread_arg* thread_info = malloc(sizeof(struct thread_arg));
    thread_info->start_idx = page_idx;
    thread_info->revolutions = REVOLUTIONS;
    thread_info->end_idx = pages.last_page_idx;
    // create thread here with arg.
    
    for(int counter = 0; counter < REVOLUTIONS; counter++)
        page_rank_single_thread();

    // results print -------------------------------------------------------
    graph_print_ranks();


    // free ----------------------------------------------------------------
    if(!str)
        free(str);
    graph_free();
}