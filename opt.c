#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
//#include "sim.h"


#define MAXLINE 256
extern int memsize; // how many elements in the coremap
extern int debug; 

extern struct frame *coremap;
int line_num; // elements in the reference string
unsigned long *array;
int location = 0;

extern char *tracefile;

//helper to return the first occurence of the frame
// input - vaddr
// returns location of where the frame is at in ref, otherwise -2 not found
int where_frame(int coreframenum){
	int i;
	for (i = (location + 1); i<line_num; i++){
		if (coreframenum == array[i]){
			return i;
		} 
	}
	return -2;
}

// helper function return the reference number of the file 
int ref_number(FILE *infp) {
	
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	int counter = 0; 

	while(fgets(buf, MAXLINE, infp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			//printf("%lx\n", vaddr);
			++counter;
		} 
	}
	// return number of lines 
	return counter;
}

// this is helper to store all the array of page_table_frames
// input - line number of how big the list virtual address should be. 
void array_info(){
	
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	int counter = 0; 

    FILE * f = fopen(tracefile, "r");
    
	while(fgets(buf, MAXLINE, f) != NULL){
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
	
			array[counter] = vaddr >> PAGE_SHIFT; //storing
			
			++counter; // plus counter for next element
		} else {
			continue;
		}
	}
	fclose(f);
}



/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i;
	int greatest = -1;
	int victim;
	for (i = 0; i<memsize; i++){
		if (coremap[i].far_occur == -2){
			return coremap[i].pte->frame;
		} 
		if (coremap[i].far_occur > greatest){
			greatest = coremap[i].far_occur;
			victim = coremap[i].pte->frame;
		} 
	}
	return victim; 
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	// using your global array of traces
	// find the next occurance and store it in far_occr  
	// core map
		
	int i; // far occur loop
	int faroccur;
	int y; // full check loop
	int full = -1; //full
	int found = -1;
	for(y = 0; y < memsize; y++) {
		if(!coremap[y].in_use) {
			full = y;
			break;
		}
		if (p->frame == coremap[y].pte->frame){
			found = 1;
			break;
		}
	}
	// coremap is full and didn't find p
	if ((full == -1) && (found == -1)){ 
		for (i = 0; i<memsize; i++){
			// location of where p happens again in reference string
			faroccur = where_frame(coremap[i].pte->frame);
			coremap[i].far_occur = faroccur;
		}	
	}
	
	location++;
	//printf("%s", tracefile);
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	// initiate the array 
	FILE *tfp;
	if((tfp = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}
	
	line_num = ref_number(tfp);
	fclose(tfp);
	array = malloc(sizeof(unsigned long)*line_num);	
	array_info(); // array is filled now with tracefile
}


