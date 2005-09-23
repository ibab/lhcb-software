#include <cstdio>
#include <cstring>
#include "RTL/rtl.h"
#include "RTL/que.h"
#define QR_success(a) ((a) > 0)

struct QENTRY : public qentry_t  {
  QENTRY() : qentry_t(), serial(0) {}
  int serial;
};

int mbm_testqhi() {
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
      printf("mbm_testqhi> Addresses: 0x%p 0x%p Ent: %p %p Head: %p %p Serial:%d \n", e, ent, ent->next, ent->prev, head.next, head.prev, ent->serial);
    }
  }
  printf("Start restoring: %p %p\n", head.next, head.prev);
  for (sc=remqhi(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqhi(&p_queue,(qentry_t**)&e), ++cnt)  {
    insqhi(e, &head);
    QENTRY* ent = add_ptr(e,-QUE_next_off);
    printf("mbm_testqhi> Addresses: 0x%p 0x%p Ent: %p %p Head: %p %p Serial:%d \n", e, ent, ent->next, ent->prev, head.next, head.prev, ent->serial);
  }
  printf("Done restoring: %p %p\n", p_queue.next, p_queue.prev);
  return 0;
}

int mbm_testqti() {
  int QUE_next_off = 0;
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
      printf("mbm_testqti> Addresses: 0x%p 0x%p 0x%p Serial:%d \n", e, &entries[c], ent, ent->serial);
    }
  }
  printf("Start restoring: %p %p\n", head.next, head.prev);
  for (sc=remqti(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqti(&p_queue,(qentry_t**)&e), ++cnt)  {
    printf("mbm_testqti> Restoring Queue element: 0x%p, %d\n", e, e->serial);
    insqti(e, &head);
  }
  printf("Done restoring: %p %p\n", p_queue.next, p_queue.prev);
  return 0;
}

extern "C" int mbm_testque2(int,char **) {
  int QUE_next_off = 0;
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
      printf(" Addresses: 0x%p 0x%p 0x%p Serial:%d \n", e, &entries[c], ent, ent->serial);
    }
  }
  printf("Start restoring: %p %p\n", head.next, head.prev);
  for (sc=remqhi(&p_queue,(qentry_t**)&e), cnt=0;QR_success(sc);sc=remqhi(&p_queue,(qentry_t**)&e), ++cnt)  {
    printf("Restoring Queue element: 0x%p, %d\n", e, e->serial);
    insqhi(e, &head);
  }
  return 0;
}

extern "C" int mbm_testque(int , char **) {
  mbm_testqhi();
  mbm_testqti();
  return 1;
}
