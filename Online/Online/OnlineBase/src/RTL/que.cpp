/*
** QUEUE ROUTINES BY TIM CHARITY - Sep 89
*/
#include "RTL/que.h"
#include "RTL/rtl.h"

typedef signed long QENT;
/*
** Insert Queue Tail with Interlock
*/
extern "C" int insqti (qentry_t *qent, qentry_t *head)  {
  char* entry  = (char*)qent;
  char* header = (char*)head;
  QENT h_next  = *(QENT*)header;
  QENT h_prev  = *(QENT*)(header+sizeof(qentry_t*));
  QENT offset  =  (QENT) (header-entry);
  /* cli() or a semaphore/mutex? */
  *(QENT*)entry     = offset;
  *(QENT*)(entry+sizeof(qentry_t*)) = h_prev+offset;
  if (h_prev)
    *(QENT*)(header+(QENT)(h_prev))=-h_prev-offset;
  else
    h_next = -offset-h_prev;
  *(QENT*)(header+sizeof(qentry_t*))=-offset;
  *(QENT*)header=h_next;
  return head->prev==head->next ? QUE_ONEENTQUE : QUE_SUCCESS;
}

/*
** Insert Queue Head with Interlock
*/
extern "C" int insqhi(qentry_t *qent, qentry_t *head) {
  char* entry  = (char*)qent;
  char* header = (char*)head;
  QENT tmp1 = *(QENT*)header;
  QENT tmp2 =  (QENT)(header-entry);
  /* cli() or a semaphore/mutex? */
  *(QENT*)entry     = tmp1+tmp2;
  *(QENT*)(entry+sizeof(qentry_t*)) = tmp2;
  *(QENT*)(header+(QENT)(tmp1)+sizeof(qentry_t*))=-tmp1-tmp2;
  *(QENT*)header    = -tmp2;
  return head->prev==head->next ? QUE_ONEENTQUE : QUE_SUCCESS;
}

/*
** Remove Queue Head with Interlock
*/
extern "C" int remqhi(qentry_t *head, qentry_t **qent)  {
  if ( head->prev == 0 && head->next == 0 )   {
    *qent = 0;
    return QUE_QUEWASEMPTY;
  }
  char* header = (char*)head;
  char* addr = (char*)qent;
  QENT tmp1 = *(QENT *)header;
  QENT tmp2 = (QENT)header+tmp1;
  QENT tmp3;
  /* cli() or a semaphore/mutex? */
  if (!tmp1)
    tmp3=(QENT) header;
  else
    tmp3=tmp2+*(QENT *)tmp2;
  *((QENT *)(tmp3+sizeof(qentry_t*)))=(QENT)header-tmp3;
  *(QENT *)header=tmp3-(QENT)header;
  *(char**)addr = (char*)(QENT *)tmp2;
  if ( head->prev == 0 && head->next == 0 )   {
    (*qent)->next = (*qent)->prev = 0;
    return QUE_QUEISEMPTY;
  }
  else if ( head->prev == head->next )  {
    (*qent)->next = (*qent)->prev = 0;
    return QUE_ONEENTQUE;
  }
  (*qent)->next = (*qent)->prev = 0;
  return QUE_SUCCESS;
}

/*
** Remove Queue Tail with Interlock
*/
extern "C" int remqti(qentry_t *head, qentry_t **qent)  {
  if ( head->prev == 0 && head->next == 0 )   {
    *qent = 0;
    return QUE_QUEWASEMPTY;
  }
  char* header = (char*)head;
  char* myaddr = (char*)qent;
  /*QENT tmp1=*header;*/
  QENT tmp5=*(QENT *)(header+sizeof(qentry_t*));
  QENT addr=(QENT)header+tmp5;
  QENT tmp2=addr+*((QENT *)(addr+sizeof(qentry_t*)));
  /* cli() or a semaphore/mutex? */
  *(QENT *)(header+sizeof(qentry_t*))=tmp2-(QENT)header;
  if (tmp2 == (QENT) header)
    *(QENT *)header=0;
  else
    *(QENT *)tmp2=(QENT)header-tmp2;
  *(char**)myaddr=(char*)(QENT *) addr;
  if ( head->prev == 0 && head->next == 0 )   {
    (*qent)->next = (*qent)->prev = 0;
    return QUE_QUEISEMPTY;
  }
  else if ( head->prev == head->next )  {
    (*qent)->next = (*qent)->prev = 0;
    return QUE_ONEENTQUE;
  }
  (*qent)->next = (*qent)->prev = 0;
  return QUE_SUCCESS;
}

qentry_t* remqent(qentry_t* e)  {
  if ( e )  {
    qentry_t* entry = 0;
    qentry_t *hd = add_ptr(e->prev,e);
    remqhi(hd, &entry);
    return entry;
  }
  return 0;
}

qentry_t *remqhead( qentry_t* head )   {
  qentry_t *entry;
  int status  = remqhi(head,&entry);
  return lib_rtl_queue_success(status) ? entry : 0;
}
