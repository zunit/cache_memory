#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// this is a struct for each node in linklist
typedef struct link_node {
    unsigned frame;
    struct link_node * next;
} node;

node * start = NULL; // beginning of linklist

// this algorithm figures out which frame to add to the beginning
void addNode(unsigned frame){
    
    node * curr = start;
    
    if (curr == NULL){ //nothing in list
        node * new_start = malloc(sizeof(node));
        new_start->frame = frame;
        new_start->next = NULL;
        start = new_start; // replace start
    }
    // we only have 1 item in list and that item isn't our frame insert
    else if ((curr->next == NULL) & (curr->frame != frame)){
        node * new_start = malloc(sizeof(node));
        new_start->frame = frame;
        new_start->next = start;
        start = new_start;
    }
    else{
        // iterate through the list
        while ((curr->next->next != NULL) && (curr->next->frame != frame)) {
            curr = curr->next;
        }
        // found frame and put it to the beginning
        if (curr->next->frame == frame) {
            node * new_start = curr->next;
            curr->next = new_start->next;
            new_start->next = start;
            start = new_start;
        } else { // add a new start
            node * new_start = malloc(sizeof(node));
            new_start->frame = frame;
            new_start->next = start;
            start = new_start;
        }
    }
}

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
    if (start == NULL){
        perror("lru error");
        exit(1);
    }
    int val;
    // 1 item in list and core map is full
    if (start->next == NULL) {
        val = start->frame;
        free(start);
        start = NULL;
        return val;
    }
    
    // iterating through list to find last one
    node *curr = start;
    
    while (curr->next->next != NULL) {
        curr = curr->next;
    }
    
    node * victim = curr->next;
    val = victim->frame;
    curr->next = victim->next;
    free(victim);
    return val;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
    // gets the frame number of the ptl entry
    unsigned frame = PGTBL_INDEX(p->frame);
    addNode(frame); // add the node into list
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
    start = NULL;
}
