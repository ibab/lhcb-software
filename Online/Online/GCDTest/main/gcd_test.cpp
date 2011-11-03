#include <dispatch/dispatch.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>

#define ITEM_VMIN       (1)
#define ITEM_VMAX       (200)
#define NR_ITEMS        (100)
static long s_calls = 0;

static void __fill_item (void *items, size_t n) {
  double *i_items = (double *)items;
  i_items[n] = n;//(ITEM_VMIN + (int)(ITEM_VMAX * ((double)rand() / RAND_MAX)));
}

static void __work_on_item (void *items, size_t n) {
  double *i_items = (double *)items;
  i_items[n] *= 1.0001;  /* Do some Computation on this Item */
  ++s_calls;
}

static void __work_on_single(void *item) {
  double *i_items = (double*)item;
  *i_items *= 1.0001;  /* Do some Computation on this Item */
  ++s_calls;
  ::sleep(10);
}

static void __print_item (void *items, size_t n) {
  double *i_items = (double *)items;
  ::printf("Results: Item %lu = %f\n", n, i_items[n]);
}

static void __barrier(void* ctxt) {
  ::printf("Barrier: Item %lu = %ld\n", (long)ctxt, s_calls);
}

int test_basics() {
  double data[NR_ITEMS];
  // Get Global Dispatch Queue
  dispatch_queue_t   queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,NULL);
  // Initialize data Elements, and run computation on each element
  dispatch_apply_f(NR_ITEMS, queue, data, __fill_item);
  for(size_t j=0; j<10000; ++j)
    dispatch_apply_f(NR_ITEMS, queue, data, __work_on_item);
  dispatch_apply_f(NR_ITEMS, queue, data, __print_item);
  return(0);
}

#include "dispatch/queue_private.h"

int test_barrier() {
  size_t j;
  double data[NR_ITEMS];
  dispatch_queue_t   queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,NULL);
  // Initialize data Elements, and run computation on each element
  dispatch_apply_f(NR_ITEMS, queue, data, __fill_item);
  for(j=0; j<10000; ++j) {
    dispatch_apply_f(NR_ITEMS, queue, data, __work_on_item);
    if ( j>0 && (j%1000) == 0 ) {
      dispatch_barrier_async_f(queue,(void*)long(j),__barrier);
    }
  }
  dispatch_barrier_async_f(queue,(void*)long(j),__barrier);
  dispatch_apply_f(NR_ITEMS, queue, data, __print_item);
  return(0);
}

int test_async() {
  double data[NR_ITEMS];
  dispatch_group_t   group = dispatch_group_create();
  dispatch_queue_t   queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,NULL);

  // Initialize data Elements, and run computation on each element
  dispatch_apply_f(NR_ITEMS, queue, data, __fill_item);
  time_t start = time(0);
  size_t j;
  for(j=0; j<2; ++j) {
    for(size_t i=0; i<NR_ITEMS; ++i)  
      dispatch_group_async_f(group,queue,data+i,__work_on_single);
  }
  ::printf("--> All jobs submitted  %ld.....\n",long(time(0)-start));
  ::dispatch_group_wait(group,DISPATCH_TIME_FOREVER);
  ::printf("--> All jobs completed  %ld.....\n",long(time(0)-start));
  ::dispatch_apply_f(NR_ITEMS, queue, data, __print_item);
  ::dispatch_group_wait(group,DISPATCH_TIME_FOREVER);
  ::dispatch_release(group);
  return(0);
}

int main(int /* argc */, const char** /* argv */) {
  //test_basics();
  //test_barrier();
  test_async();
  return 0;
}
