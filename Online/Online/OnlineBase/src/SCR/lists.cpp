/*
  lists.c
  Created           : 18-OCT-1989 by Christian Arnault
*/
 
/*---------------------------------------------------------------------------*/
#define __LISTS_C__
#include <cstdlib>
#include "SCR/lists.h"

/*---------------------------------------------------------------------------*/
void list_init (Linked_list* _list) {
  _list->first = (Link*) 0;
  _list->last  = (Link*) 0;
}
 
/*---------------------------------------------------------------------------*/
Link* list_add_entry (Linked_list* _list, int _size)    {
  Link* s = (Link*) list_malloc (_size);
  if (_list->first)  {
    _list->last->next = s;
  }
  else  {
    _list->first = s;
  }
  s->prev   = _list->last;
  s->next   = (Link*) 0;
  s->father = _list;
  _list->last = s;
  return (s);
}

void list_connect_entry (Link* _entry,  Linked_list* _list) {
  if (_list->first)  {
    _list->last->next = _entry;
  }
  else  {
    _list->first = _entry;
  }
  _entry->prev   = _list->last;
  _entry->next   = (Link*) 0;
  _entry->father = _list;
  _list->last = _entry;
}

Link* list_insert_entry (Link* _before, Link* _after, int _size)    {
  /* syntax check */
  if (!_before && !_after) return 0;
  if (!_before) _before = _after->prev;
  if (!_after) _after  = _before->next;
  
  Linked_list* list = (_before) ? _before->father : _after->father;
  Link* s = (Link*) list_malloc (_size);
  
  _before ? _before->next = s : list->first = s;  
  _after  ? _after->prev  = s : list->last = s;  
  s->prev   = _before;
  s->next   = _after;
  s->father = list;  
  return (s);
}

void list_remove_entry ( Link* _entry )   {
  Linked_list* list = _entry->father;
  if (!list) return;
  
  Link* prev = _entry->prev;
  Link* next = _entry->next;  
  prev ? prev->next = next : list->first = next;  
  next ? next->prev = prev : list->last  = prev;  
  free (_entry);
}

void list_move_entry (Link *_entry, Link *_before, Link *_after)    {
  list_transfer_entry (_entry, _entry->father, _before, _after);
}

void list_disconnect_entry (Link* _entry)   {
  Linked_list* list = _entry->father;
  if (!list) return;
  
  Link* prev = _entry->prev;
  Link* next = _entry->next;
  prev ? prev->next = next : list->first = next;
  next ? next->prev = prev : list->last = prev;
  _entry->father = (Linked_list*) 0;
}

/*---------------------------------------------------------------------------*/
void list_transfer_entry (Link* _entry, Linked_list* _list,
                          Link* _before, Link* _after)
/*---------------------------------------------------------------------------*/
{
  /* syntax check */
  if (_before)  {
    if (_list != _before->father) return;
    if (_before == _entry) return;
  }
  if (_after)  {
    if (_list != _after->father) return;
    if (_after == _entry) return;
  }
  if (_before && _after)  {
    if (_before->next != _after || _before != _after->prev) return;
  }

  list_disconnect_entry (_entry);
  
  if (!_before && !_after) _after = _list->first;
  if (!_before && _after) _before = _after->prev;
  if (!_after && _before) _after  = _before->next;
  
  //Linked_list* list = _entry->father;

  /* Now put entry back. */
  
  if (_before) _before->next = _entry;
  else _list->first = _entry;
  
  if (_after) _after->prev = _entry;
  else _list->last = _entry;
  
  _entry->next   = _after;
  _entry->prev   = _before;
  _entry->father = _list;
}


/*---------------------------------------------------------------------------*/
void list_transfer (Linked_list* _from, Linked_list* _to) {
  Link* l = _from->first;
  if (l)  {
    if (_to->first) return;
    while (l)    {
      l->father = _to;
      l = l->next;
    }
    *_to = *_from;
    _from->first = _from->last = (Link*) 0;
  }
}

/*----------------------------------------------------------------------------*/
void list_convert (Linked_list* list)   {
  Link* s = list->first;
  while (s)  {
    listr_set_prev   (s, s->prev);
    listr_set_next   (s, s->next);
    listr_set_father (s, s->father);
    s = listr_next(s);
  }
  listr_set_first (list, list->first);
  listr_set_last  (list, list->last);
}

/*----------------------------------------------------------------------------*/
Link* list_first (Linked_list* list)
/*----------------------------------------------------------------------------*/
{
  return (list->first);
}

/*----------------------------------------------------------------------------*/
Link* list_last ( Linked_list* list)
/*----------------------------------------------------------------------------*/
{
  return (list->last);
}

/*----------------------------------------------------------------------------*/
Link* list_prev (Link* entry)
/*----------------------------------------------------------------------------*/
{
  return (entry->prev);
}

/*----------------------------------------------------------------------------*/
Link* list_next (Link* entry)
/*----------------------------------------------------------------------------*/
{
  return (entry->next);
}

/*----------------------------------------------------------------------------*/
Link* list_father (Link* entry)
/*----------------------------------------------------------------------------*/
{
  return (Link *)(entry->father);
}

/*---------------------------------------------------------------------------*/
char* list_malloc ( int _size)
/*---------------------------------------------------------------------------*/
{
  char* ptr;
  if ( _size <= 0 ) return(0);  
  if ((ptr = (char*) malloc (_size))) return (ptr);
  exit(1);
}
