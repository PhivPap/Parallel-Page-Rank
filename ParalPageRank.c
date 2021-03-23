// page links: HOLD IDX OF PAGE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "symtable.h"

#define REVOLUTIONS 50
#define INIT_GRAPH_NODES 14000

typedef long long int int64;

struct Page{
    int64 id;
    double rank;
    double rank_change;
    struct Page_link* links; // list of links to other pages.
    int links_count;
    pthread_mutex_t lock;
};

struct Page_link{
    size_t page_idx;
    struct Page* page;
    struct Page_link* next;
};

struct Graph{ // singleton struct to hold all nodes (pages) of the graph
    struct Page* array;
    size_t array_size;
    size_t last_page_idx;
};

struct thread_arg{
    int64 start_idx, end_idx;
};

struct Graph pages;
SymTable_T lookup_table;
pthread_t tids[4];
pthread_barrier_t barrier;



void graph_init(void){
    lookup_table = SymTable_new();
    pages.last_page_idx = -1;
    pages.array_size = INIT_GRAPH_NODES;
    pages.array = malloc(INIT_GRAPH_NODES * sizeof(struct Page));
    for(int64 idx = 0; idx < pages.array_size; idx++){
        pages.array[idx].links = NULL;
        pages.array[idx].rank = 1;
        pages.array[idx].rank_change = 0;
        pages.array[idx].links_count = 0;
    }
}

void graph_free(void){
    for(int64 idx = 0; idx < pages.array_size; idx++){
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
        pages.array[idx].rank = 1;
        pages.array[idx].rank_change = 0;
        pages.array[idx].links_count = 0;
    }
}

void graph_add_page_link(int64 page_id, int64 page_link){
    int64 page1_idx = SymTable_get(lookup_table, page_id);
    struct Page* page1 = NULL;
    if(page1_idx == -1){
        if(pages.last_page_idx == pages.array_size - 1)
            graph_double_nodes();
        page1_idx = ++(pages.last_page_idx);
        page1 = &pages.array[pages.last_page_idx];
        page1->id = page_id;
        SymTable_put(lookup_table, page_id, pages.last_page_idx);
    }


    int64 page2_idx = SymTable_get(lookup_table, page_link);
    struct Page* page2 = NULL;
    if(page2_idx == -1){
        if(pages.last_page_idx == pages.array_size - 1)
            graph_double_nodes();
        page2_idx = ++(pages.last_page_idx);
        page2 = &pages.array[pages.last_page_idx];
        page2->id = page_link;
        SymTable_put(lookup_table, page_link, pages.last_page_idx);
    }

    if(page1 == NULL){
        page1 = &pages.array[page1_idx];
    }
    if(page2 == NULL){
        page2 = &pages.array[page2_idx];
    }

    struct Page_link* new_page_link = malloc(sizeof(struct Page_link));
    new_page_link->page_idx = page2_idx;
    new_page_link->next = page1->links;
    page1->links = new_page_link;
    page1->links_count++;
}

void graph_init_all_locks(void){
    for(size_t idx = 0; idx <= pages.last_page_idx; idx++){
        pthread_mutex_init(&pages.array[idx].lock, NULL);
    }
}

void graph_make_links_ptrs(void){
    for(size_t idx = 0; idx <= pages.last_page_idx; idx++){
        struct Page_link* ptr = pages.array[idx].links;
        while(ptr){
            ptr->page = &pages.array[ptr->page_idx];
            ptr = ptr->next;
        }
    }
}

/* For debugging reasons */
void graph_print(){
    for(int64 idx = 0; idx < pages.array_size; idx++){
        printf("%lld:\t", idx);
        struct Page_link* ptr = pages.array[idx].links;
        while(ptr){
            printf("%lld\t", ptr->page->id);
            ptr = ptr->next;
        }
        printf("\n");
    }
}

void graph_print_ranks(){
    printf("#node,pagerank\n");
    for(int64 idx = 0; idx <= pages.last_page_idx; idx++){
        printf("%lld,%.3f\n", pages.array[idx].id, pages.array[idx].rank);
    }
}

void page_rank_single_thread(){
    int64 idx = 0;
    while(idx <= pages.last_page_idx){
        if(!pages.array[idx].links){
            idx++;
            continue;
        }
        struct Page_link* ptr = pages.array[idx].links;
        // increases rank of all neighboring pages. 
        // (pages that the particular page has links to)
        double normalized_rank = pages.array[idx].rank / (double)pages.array[idx].links_count;
        double rank_segment = 0.85 * normalized_rank;
        pages.array[idx].rank *= 0.15;

        while(ptr){ 
            ptr->page->rank_change += rank_segment;
            ptr = ptr->next;
        }  

        idx++;
    }

    idx = 0;
    while(idx <= pages.last_page_idx){
        pages.array[idx].rank += pages.array[idx].rank_change;
        pages.array[idx].rank_change = 0;
        idx++;
    }
}

void* page_rank_multi_thread(void* _arg){
    struct thread_arg* arg = (struct thread_arg*)_arg;

    for(int rev = 0; rev < REVOLUTIONS; rev++){
    	int64 idx = arg->start_idx;
	    while(idx <= arg->end_idx){
	        if(!pages.array[idx].links){
	            idx++;
	            continue;
	        }
	        struct Page_link* ptr = pages.array[idx].links;
	        // increases rank of all neighboring pages. 
	        // (pages that the particular page has links to)
	        double normalized_rank = pages.array[idx].rank / (double)pages.array[idx].links_count;
	        double rank_segment = 0.85 * normalized_rank;
	        pages.array[idx].rank *= 0.15;

	        while(ptr){
	           pthread_mutex_lock(&(ptr->page->lock));
	           ptr->page->rank_change += rank_segment;
	           pthread_mutex_unlock(&(ptr->page->lock));
	           ptr = ptr->next;
	        }  

	        idx++;
	    }
	   pthread_barrier_wait(&barrier);

	    idx = arg->start_idx;
	    while(idx <= arg->end_idx){
	        pages.array[idx].rank += pages.array[idx].rank_change;
	        pages.array[idx].rank_change = 0;
	        idx++;
	    }

	   pthread_barrier_wait(&barrier);
	}
}

/* 
    argv[1] = <count of threads> 
*/
int main(int argc, const char* argv[]){

	int thread_idx = 0; // to manage array of pthreads.
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

   
    
    // graph parsing -------------------------------------------------------
    graph_init();
    while(getline(&str, &size, stdin) != -1){ // currently using stdin
        if(str[0] == '#') // skip comments.
            continue;
        sscanf(str, "%lld %lld", &page_id, &page_link);
        graph_add_page_link(page_id, page_link);
    }
    SymTable_free(lookup_table);
    graph_init_all_locks();
    graph_make_links_ptrs();

    // barrier init --------------------------------------------------------
    pthread_barrier_init(&barrier, NULL, thread_count);
    // thread generating ---------------------------------------------------
    int64 pages_per_thread = pages.last_page_idx / thread_count;
    int64 page_idx = 0;
    for(int i = 0; i < thread_count - 1; i++){
        struct thread_arg* thread_info = malloc(sizeof(struct thread_arg));
        thread_info->start_idx = page_idx;
        page_idx += pages_per_thread;
        thread_info->end_idx = page_idx - 1;
        // create thread here with arg.
        pthread_create(&tids[thread_idx++], NULL, page_rank_multi_thread, thread_info);
    }
    
    struct thread_arg* thread_info = malloc(sizeof(struct thread_arg));
    thread_info->start_idx = page_idx;
    thread_info->end_idx = pages.last_page_idx;
    // create thread here with arg.
    pthread_create(&tids[thread_idx++], NULL, page_rank_multi_thread, thread_info);


    for(int i = 0; i < thread_count; i++){
    	pthread_join(tids[i], NULL);
    }
    // for(int counter = 0; counter < REVOLUTIONS; counter++)
    //     page_rank_single_thread();

    // results print -------------------------------------------------------
    graph_print_ranks();


    // free ----------------------------------------------------------------
    if(!str)
        free(str);
    graph_free();
}
