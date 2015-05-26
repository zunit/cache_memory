#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clockhand;
unsigned *framepages;
int *references;

void helper_clock(unsigned frame){
	int i;
	for (i = 0; i < memsize; i++){
		
		if (frame == framepages[i]){
			references[i] = 1;
			return ;
		}
	}
	references[clockhand] = 1;
	framepages[clockhand] = frame;
	clockhand += 1;

}

int clock_evict() {
	while (references[clockhand] != 0){
		references[clockhand] = 0;
		clockhand = (clockhand + 1) % memsize;
	}
	int frame = (int) framepages[clockhand];
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	unsigned framepages = PGTBL_INDEX(p->frame);
	helper_clock(framepages);
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	int * reflist = malloc(sizeof(int)*memsize);
	unsigned * pglist = malloc(sizeof(unsigned)*memsize);
	framepages = pglist;
	references = reflist;
	clockhand = 0;
	int j = 0;
	for (j = 0; j<memsize; j++){
		framepages[j] = 0;
		references[j] = 0;
	}
}
