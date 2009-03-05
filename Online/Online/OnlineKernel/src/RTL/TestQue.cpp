#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"
#include "RTL/que.h"
#define QR_success(a) ((a) > 0)

struct QENTRY : public qentry_t  {
  QENTRY() : qentry_t(), serial(0) {}
  size_t serial;
};

extern "C" int rtl_testqhi() {
  int QUE_next_off = 0;
  QENTRY   entries[16];
  qentry_t head(0,0);
  qentry_t p_queue(0,0);
  ::memset(entries,0,sizeof(entries));

  byte_offset(qentry_t,next,QUE_next_off);
  int sc, cnt;
  for(size_t i=0; i<sizeof(entries)/sizeof(entries[0]); ++i)  {
    entries[i].serial = i;
    insqti(&entries[i], &head);
  }

  QENTRY* e;
  for(sc = remqhi(&head,(qentry_t**)&e), cnt=0; QR_success(sc); sc = remqhi(&head,(qentry_t**)&e), ++cnt )  {
    insqhi(e, &p_queue);
    QENTRY* ent = add_ptr(e,-QUE_next_off);
    int c = sizeof(entries)/sizeof(entries[0])-cnt;
    if ( ent != &entries[c] )  {
      ::printf("rtl_testqhi> Addresses: %p %p Ent: %p %p Head: %p %p Serial:%ld \n", 
       (void*)e, (void*)ent, (void*)ent->next, (void*)ent->prev, 
       (void*)head.next, (void*)head.prev, long(ent->serial));
    }
  }
  ::printf("Start restoring: %p %p\n", (void*)head.next, (void*)head.prev);
  for (sc=remqhi(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqhi(&p_queue,(qentry_t**)&e), ++cnt)  {
    insqhi(e, &head);
    QENTRY* ent = add_ptr(e,-QUE_next_off);
    ::printf("rtl_testqhi> Addresses: %p %p Ent: %p %p Head: %p %p Serial:%ld \n", 
       (void*)e, (void*)ent, (void*)ent->next, (void*)ent->prev, 
       (void*)head.next, (void*)head.prev, long(ent->serial));
  }
  ::printf("Done restoring: %p %p\n", (void*)p_queue.next, (void*)p_queue.prev);
  return 0;
}

extern "C" int rtl_testqti() {
  int64_t QUE_next_off = 0;
  QENTRY   entries[16];
  qentry_t head(0,0);
  qentry_t p_queue(0,0);
  ::memset(entries,0,sizeof(entries));

  byte_offset(qentry_t,next,QUE_next_off);
  int sc, cnt;
  for(size_t i=0; i<sizeof(entries)/sizeof(entries[0]); ++i)  {
    entries[i].serial = i;
    insqhi(&entries[i], &head);
  }

  QENTRY* e;
  for(sc = remqti(&head,(qentry_t**)&e), cnt=0; QR_success(sc); sc = remqti(&head,(qentry_t**)&e), ++cnt )  {
    insqti(e, &p_queue);
    QENTRY* ent = add_ptr(e,-QUE_next_off);
    int c = sizeof(entries)/sizeof(entries[0])-cnt;
    if ( ent != &entries[c] )  {
      ::printf("rtl_testqti> Addresses: %p %p %p Serial:%ld \n", 
       (void*)e, (void*)&entries[c], (void*)ent, long(ent->serial));
    }
  }
  printf("Start restoring: %p %p\n", (void*)head.next, (void*)head.prev);
  for (sc=remqti(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqti(&p_queue,(qentry_t**)&e), ++cnt)  {
    ::printf("rtl_testqti> Restoring Queue element: %p, %ld\n", (void*)e, long(e->serial));
    insqti(e, &head);
  }
  ::printf("Done restoring: %p %p\n", (void*)p_queue.next, (void*)p_queue.prev);
  return 0;
}

extern "C" int rtl_testque2(int,char **) {
  int64_t QUE_next_off = 0;
  QENTRY   entries[128];
  qentry_t head(0,0);
  qentry_t p_queue(0,0);
  ::memset(entries,0,sizeof(entries));

  byte_offset(qentry_t,next,QUE_next_off);
  int sc, cnt;
  for(size_t i=0; i<sizeof(entries)/sizeof(entries[0]); ++i)  {
    entries[i].serial = i;
    insqhi(&entries[i], &head);
  }

  QENTRY* e;
  for(sc = remqhi(&head,(qentry_t**)&e), cnt=0; QR_success(sc); sc = remqhi(&head,(qentry_t**)&e), ++cnt )  {
    insqhi(e, &p_queue);
    QENTRY* ent = add_ptr(e,-QUE_next_off);
    int c = sizeof(entries)/sizeof(entries[0])-cnt;
    if ( ent != &entries[c] )  {
      ::printf(" Addresses: %p %p %p Serial:%ld \n", 
       (void*)e, (void*)&entries[c], (void*)ent, long(ent->serial));
    }
  }
  ::printf("Start restoring: %p %p\n", (void*)head.next, (void*)head.prev);
  for (sc=remqhi(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqhi(&p_queue,(qentry_t**)&e), ++cnt)  {
    ::printf("Restoring Queue element: %p, Serial:%ld\n", (void*)e, long(e->serial));
    insqhi(e, &head);
  }
  return 0;
}

extern "C" int rtl_testque(int , char **) {
  rtl_testqhi();
  rtl_testqti();
  printf("[RTL Test]  rtl_testque>   Successfully completed.\n");
  return 0;
}
