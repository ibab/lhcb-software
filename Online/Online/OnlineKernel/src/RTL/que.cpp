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
  QENT h_prev  = *(QENT*)(header+4);
  QENT offset  =  (QENT) (header-entry);
  /* cli() or a semaphore/mutex? */
  *(QENT*)entry     = offset;
  *(QENT*)(entry+4) = h_prev+offset;
  if (h_prev)
    *(QENT*)(header+(QENT)(h_prev))=-h_prev-offset;
  else
    h_next = -offset-h_prev;
  *(QENT*)(header+4)=-offset;
  *(QENT*)header=h_next;
  return head->prev==head->next ? QUE_ONEENTQUE : 1;
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
  *(QENT*)(entry+4) = tmp2;
  *(QENT*)(header+(QENT)(tmp1)+4)=-tmp1-tmp2;
  *(QENT*)header    = -tmp2;
  return head->prev==head->next ? QUE_ONEENTQUE : 1;
}

/*
** Remove Queue Head with Interlock
*/
extern "C" int remqhi(qentry_t *head, qentry_t **qent)  {
  if ( head->prev == 0 && head->next == 0 )   {
    *qent = 0;
    return QUE_SUCCESS;
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
  *((QENT *)(tmp3+4))=(QENT)header-tmp3;
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
  return 1;
}

/*
** Remove Queue Tail with Interlock
*/
extern "C" int remqti(qentry_t *head, qentry_t **qent)  {
  if ( head->prev == 0 && head->next == 0 )   {
    *qent = 0;
    return QUE_SUCCESS;
  }
  char* header = (char*)head;
  char* myaddr = (char*)qent;
  /*QENT tmp1=*header;*/
  QENT tmp5=*(QENT *)(header+4);
  QENT addr=(QENT)header+tmp5;
  QENT tmp2=addr+*((QENT *)(addr+4));
  /* cli() or a semaphore/mutex? */
  *(QENT *)(header+4)=tmp2-(QENT)header;
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
  return 1;
}

qentry_t* remqent(qentry_t* e)  {
  if ( e )  {
    qentry_t* dummy = 0;
    qentry_t *hd = add_ptr(e->prev,&e->next);
    remqhi(hd, &dummy);
    return dummy;
  }
  return 0;
}
