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

static void *arena = NULL;
static Node list[MAX_ALLOC];
static int last_node;
static enum ALGORITHM alg;

int mavalloc_init( size_t size, enum ALGORITHM algorithm ) {
  // Return -1 if a size is less than 0
  if(size < 0)
    return -1;
  
  arena = malloc(ALIGN4(size));
  // Return -1 if the allocation fails
  if(!arena)
    return -1;
  
  // Initialize nodes
  for(int i = 0; i < MAX_ALLOC; i++) {
    list[i].size = 0;
    list[i].in_use = 0;
    list[i].arena = NULL;
    list[i].next = -1;
    list[i].prev = -1;
    list[i].type = H;
  }
  
  // Setup first node to have entire arena
  list[0].size = size;
  list[0].in_use = 1;
  list[0].arena = arena;
  
  last_node = 0;
  alg = algorithm;
  return 0;
}

void mavalloc_destroy( ) {
  free(arena);
  arena = NULL;
  return;
}

int find_next_empty() {
  for(int i = 0; i < MAX_ALLOC; i++) {
    if(!list[i].in_use)
      return i;
  }
  return -1;
}

void * mavalloc_alloc_first_fit( size_t size ) {
  int next = 0;
  do {
    if(list[next].in_use && list[next].type == H && size <= list[next].size) {
      int next_empty = find_next_empty();
      // Too many allocations, no empty nodes
      if(next_empty == -1) {
        return NULL;
      }
      list[next_empty].next = list[next].next;
      list[next_empty].prev = next;
      list[next_empty].size = list[next].size - size;
      list[next_empty].arena = list[next].arena + size;
      list[next_empty].type = H;
      list[next_empty].in_use = 1;
      
      list[next].next = next_empty;
      list[next].size = size;
      list[next].type = P;
      return list[next].arena;
    }
    next = list[next].next;
  } while(next != -1);
  
  // only return NULL on failure
  return NULL;
}

void * mavalloc_alloc_next_fit( size_t size ) {
  int next = last_node;
  
  // Attempt 1 (from last_node to end of list)
  do {
    if(list[next].in_use && list[next].type == H && size <= list[next].size) {
      if(list[next].size > size) {
        int next_empty = find_next_empty();
        // Too many allocations, no empty nodes
        if(next_empty == -1) {
          return NULL;
        }
        
        list[next_empty].next = list[next].next;
        list[next_empty].prev = next;
        list[next_empty].size = list[next].size - size;
        list[next_empty].arena = list[next].arena + size;
        list[next_empty].type = H;
        list[next_empty].in_use = 1;
        
        list[next].next = next_empty;
      }
      list[next].size = size;
      list[next].type = P;
      
      last_node = next;
      
      return list[next].arena;
    }
    next = list[next].next;
  } while(next != -1);
  
  // Attempt 2 (from beginning of list to last_node)
  next = 0;
  do {
    if(list[next].in_use && list[next].type == H && size <= list[next].size) {
      if(list[next].size > size) {
        int next_empty = find_next_empty();
        // Too many allocations, no empty nodes
        if(next_empty == -1) {
          return NULL;
        }
        list[next_empty].next = list[next].next;
        list[next_empty].prev = next;
        list[next_empty].size = list[next].size - size;
        list[next_empty].arena = list[next].arena + size;
        list[next_empty].type = H;
        list[next_empty].in_use = 1;
        
        list[next].next = next_empty;
      }
      
      list[next].size = size;
      list[next].type = P;
      
      last_node = next;
      
      return list[next].arena;
    }
    next = list[next].next;
  } while(next != -1 && next != last_node);
  
  // only return NULL on failure
  return NULL;
}

void * mavalloc_alloc_best_fit( size_t size ) {
  int next = 0, best_idx = -1;
  size_t best_size;
  do {
    if(list[next].in_use && list[next].type == H && size <= list[next].size && (best_idx == -1 || list[next].size - size < best_size)) {
      best_idx = next;
      best_size = list[next].size - size;
    }
    next = list[next].next;
  } while(next != -1);
  
  // only return NULL on failure
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
  int next = 0, worst_idx = -1;
  size_t worst_size = 0;
  do {
    if(list[next].in_use && list[next].type == H && size <= list[next].size && (worst_idx == -1 || list[next].size - size > worst_size)) {
      worst_idx = next;
      worst_size = list[next].size - size;
    }
    next = list[next].next;
  } while(next != -1);
  
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
  if(!arena)
    return NULL;
  if(size < 0)
    return NULL;
  // size = ALIGN4(size);
  // return mavalloc_alloc_first_fit(size);
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
  if(!ptr)
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
        list[next].next = -1;
        list[next].prev = -1;
        
        next = list[next].next;
      }
      if(prev != -1 && list[prev].type == H) {
        list[prev].next = next;
        list[prev].size += list[curr].size;
        list[next].prev = prev;
        list[curr].in_use = 0;
        list[curr].next = -1;
        list[curr].prev = -1;
      }
      break;
    }
    curr = list[curr].next;
  } while(curr != -1);
}

int mavalloc_size( ) {
  if(!arena)
    return 0;
  int number_of_nodes = 0;
  
  for(int i = 0; i < MAX_ALLOC; i++) {
    if(list[i].in_use == 1)
        number_of_nodes++;
  }
  // int next = 0;
  // while(next != -1 && list[next].type == P) {
  //   number_of_nodes++;
  //   next = list[next].next;
  // } 

  return number_of_nodes;
}
