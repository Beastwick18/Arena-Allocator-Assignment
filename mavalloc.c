// The MIT License (MIT)
// 
// Copyright (c) 2021, 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES UTA OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_ALLOC 10000

enum TYPE {
  H = 0,
  P
};

typedef struct {
  size_t size;
  int in_use;
  enum TYPE type;
  void *arena;
  int next;
  int prev;
} Node;

static void *arena = NULL;
static Node list[MAX_ALLOC];
static int last_node;
static enum ALGORITHM alg;

int mavalloc_init( size_t size, enum ALGORITHM algorithm ) {
  // Return -1 if a size is less than 0
  if(size < 0)
    return -1;
  
  size = ALIGN4(size);
  arena = malloc(size);
  // Return -1 if the allocation fails
  if(!arena)
    return -1;
  
  memset(list, 0, MAX_ALLOC * sizeof(Node));
  
  // Setup first node to have entire arena
  list[0].size = size;
  list[0].in_use = 1;
  list[0].arena = arena;
  list[0].next = -1;
  list[0].prev = -1;
  list[0].type = H;
  
  last_node = 0;
  alg = algorithm;
  return 0;
}

void mavalloc_destroy( ) {
  free(arena);
  arena = NULL;
}

int find_next_empty() {
  for(int i = 0; i < MAX_ALLOC; i++) {
    if(!list[i].in_use)
      return i;
  }
  return -1;
}

void * mavalloc_alloc_first_fit( size_t size ) {
  int current = 0;
  do {
    if(list[current].in_use && list[current].type == H && size <= list[current].size) {
      // If there will still be space left over after taking up the
      // requested allocations space, create a new node in the list
      // just after this one containing that left over space
      if(list[current].size > size) {
        // Find the next node in the list that is not being used
        int next_empty = find_next_empty();
        // Too many allocations, no empty nodes
        if(next_empty == -1) {
          return NULL;
        }
        
        list[next_empty].next = list[current].next;
        list[next_empty].prev = current;
        // Set its size to the amount left over after allocation
        list[next_empty].size = list[current].size - size;
        // Change arena starting point to be after our block
        list[next_empty].arena = list[current].arena + size;
        list[next_empty].type = H;
        list[next_empty].in_use = 1;
        
        list[current].next = next_empty;
      }
      list[current].size = size;
      list[current].type = P;
      
      return list[current].arena;
    }
    current = list[current].next;
  } while(current != -1);
  
  // only return NULL on failure
  return NULL;
}

void * mavalloc_alloc_next_fit( size_t size ) {
  // Start at last node we left off on in previous allocation
  int current = last_node;
  
  // Attempt to find a node that can contain our allocation,
  // starting from the last node we left off on in our
  // previous allocation
  do {
    // If the current node is in use, is a hole, and can contain our allocation,
    // then use it for the allocation
    if(list[current].in_use && list[current].type == H && size <= list[current].size) {
      // If there will still be space left over after taking up the
      // requested allocations space, create a new node in the list
      // just after this one containing that left over space
      if(list[current].size > size) {
        // Find the next node in the list that is not being used
        int next_empty = find_next_empty();
        // Too many allocations, no empty nodes
        if(next_empty == -1) {
          return NULL;
        }
        
        list[next_empty].next = list[current].next;
        list[next_empty].prev = current;
        list[next_empty].size = list[current].size - size;
        list[next_empty].arena = list[current].arena + size;
        list[next_empty].type = H;
        list[next_empty].in_use = 1;
        
        list[current].next = next_empty;
      }
      list[current].size = size;
      list[current].type = P;
      
      last_node = current;
      
      return list[current].arena;
    }
    current = list[current].next;
  } while(current != -1);
  
  // If we've made it here, that means we were unable to find a node between
  // the last node we left off on and the end of the list. Now, we will try
  // to find a node starting from the beginning of the list up to the last
  // node we left off on (the part of the list we haven't checked yet)
  current = 0;
  do {
    // If the current node is in use, is a hole, and can contain our allocation,
    // then use it for the allocation
    if(list[current].in_use && list[current].type == H && size <= list[current].size) {
      // If there will still be space left over after taking up the
      // requested allocations space, create a new node in the list
      // just after this one containing that left over space
      if(list[current].size > size) {
        int next_empty = find_next_empty();
        // Too many allocations, no empty nodes
        if(next_empty == -1) {
          return NULL;
        }
        list[next_empty].next = list[current].next;
        list[next_empty].prev = current;
        list[next_empty].size = list[current].size - size;
        list[next_empty].arena = list[current].arena + size;
        list[next_empty].type = H;
        list[next_empty].in_use = 1;
        
        list[current].next = next_empty;
      }
      
      list[current].size = size;
      list[current].type = P;
      
      last_node = current;
      
      return list[current].arena;
    }
    current = list[current].next;
  } while(current != -1 && current != last_node);
  
  // Return NULL as we were unable to find a block that can hold our allocation
  return NULL;
}

void * mavalloc_alloc_best_fit( size_t size ) {
  int current = 0, best_idx = -1;
  size_t best_size;
  do {
    // If the current node is in use, is a hold, is large enough to hold our allocation,
    // and either the best_idx is -1 (meaning we have not yet found a spot for our allocation)
    // or the amount of space left over after allocating in this block is less than 
    // our current "best size", select this as the new "best" block
    if(list[current].in_use && list[current].type == H && size <= list[current].size &&
        (best_idx == -1 || list[current].size - size < best_size)) {
      best_idx = current;
      best_size = list[current].size - size;
      if(best_size == 0)
        break;
    }
    // Move to next node in list
    current = list[current].next;
  } while(current != -1);
  
  // Return NULL if we were unable to find a block that can contain our allocation
  if(best_idx == -1)
    return NULL;
  
  if(list[best_idx].size > size) {
    int next_empty = find_next_empty();
    // Too many allocations, no empty nodes
    if(next_empty == -1)
      return NULL;
    
    list[next_empty].next = list[best_idx].next;
    list[next_empty].prev = best_idx;
    list[next_empty].size = list[best_idx].size - size;
    list[next_empty].arena = list[best_idx].arena + size;
    list[next_empty].type = H;
    list[next_empty].in_use = 1;
    
    list[best_idx].next = next_empty;
    list[best_idx].size = size;
  }
  
  list[best_idx].type = P;
  
  return list[best_idx].arena;
}

void * mavalloc_alloc_worst_fit( size_t size ) {
  int current = 0, worst_idx = -1;
  size_t worst_size = 0;
  do {
    // If the current node is in use, is a hold, is large enough to hold our allocation,
    // and either the worst_idx is -1 (meaning we have not yet found a spot for our allocation)
    // or the amount of space left over after allocating in this block is greater than 
    // our current "worst size", select this as the new "worst" block
    if(list[current].in_use && list[current].type == H && size <= list[current].size &&
        (worst_idx == -1 || list[current].size - size > worst_size)) {
      worst_idx = current;
      worst_size = list[current].size - size;
    }
    current = list[current].next;
  } while(current != -1);
  
  // only return NULL on failure
  if(worst_idx == -1)
    return NULL;
  
  if(list[worst_idx].size > size) {
    int next_empty = find_next_empty();
    // Too many allocations, no empty nodes
    if(next_empty == -1)
      return NULL;
    
    list[next_empty].next = list[worst_idx].next;
    list[next_empty].prev = worst_idx;
    list[next_empty].size = list[worst_idx].size - size;
    list[next_empty].arena = list[worst_idx].arena + size;
    list[next_empty].type = H;
    list[next_empty].in_use = 1;
    
    list[worst_idx].next = next_empty;
  }
  list[worst_idx].size = size;
  list[worst_idx].type = P;
  
  return list[worst_idx].arena;
}

void * mavalloc_alloc( size_t size ) {
  // If arena is NULL, we have destroyed it or not yet initialized.
  // The allocation will fail in this case
  if(!arena)
    return NULL;
  
  // Make sure size is 4 byte aligned
  size = ALIGN4(size);
  
  // Allocation fails if size is not positive
  if(size <= 0)
    return NULL;
  
  switch(alg) {
    case FIRST_FIT:
      return mavalloc_alloc_first_fit(size);
    case NEXT_FIT:
      return mavalloc_alloc_next_fit(size);
    case BEST_FIT:
      return mavalloc_alloc_best_fit(size);
    case WORST_FIT:
      return mavalloc_alloc_worst_fit(size);
  }
  return NULL;
}

void mavalloc_free( void * ptr ) {
  if(!arena || !ptr)
    return;
  
  int curr = 0;
  do {
    if(list[curr].arena == ptr) {
      int prev = list[curr].prev;
      int next = list[curr].next;
      list[curr].type = H;
      if(next != -1 && list[next].type == H) {
        list[curr].next = list[next].next;
        if(list[next].next != -1)
          list[list[next].next].prev = curr;
        list[curr].size += list[next].size;
        list[next].in_use = 0;
        // list[next].next = -1;
        // list[next].prev = -1;
        
        next = list[next].next;
      }
      if(prev != -1 && list[prev].type == H) {
        list[prev].next = next;
        list[prev].size += list[curr].size;
        list[next].prev = prev;
        list[curr].in_use = 0;
        // list[curr].next = -1;
        // list[curr].prev = -1;
      }
      return;
    }
    curr = list[curr].next;
  } while(curr != -1);
}

int mavalloc_size( ) {
  if(!arena)
    return 0;
  int number_of_nodes = 0;
  
  // for(int i = 0; i < MAX_ALLOC; i++) {
  //   if(list[i].in_use == 1)
  //       number_of_nodes++;
  // }
  int next = 0;
  while(next != -1) {
    number_of_nodes++;
    next = list[next].next;
  } 

  return number_of_nodes;
}
