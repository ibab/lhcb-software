/*
  LISTR.C
  Created           :  1-DEC-1989 by Christian Arnault
*/

#define __LISTS_C__
#include "SCR/lists.h"
#include <cstdlib>


void listr_init (Linked_list* _list)  {
  _list->first = (Link*) 0;
  _list->last  = (Link*) 0;
}

Link* listr_add_entry (Linked_list* _list, int _size) {
  Link* s = (Link*) list_malloc (_size);
  if (_list->first)
  {
    listr_set_next (listr_last(_list), s);
  }
  else
  {
    listr_set_first (_list, s);
  }
  listr_set_prev   (s, listr_last(_list));
  listr_set_next   (s, 0);
  listr_set_father (s, _list);
  listr_set_last   (_list, s);

  return (s);
}

void listr_connect_entry (Link* _entry, Linked_list* _list) {
  if (_list->first)  {
    listr_set_next (listr_last(_list), _entry);
  }
  else  {
    listr_set_first (_list, _entry);
  }
  listr_set_prev   (_entry, listr_last(_list));
  listr_set_next   (_entry, 0);
  listr_set_father (_entry, _list);
  listr_set_last   (_list, _entry);
}

/*---------------------------------------------------------------------------*/
Link* listr_insert_entry (Link* _before, Link* _after, int _size)
/*---------------------------------------------------------------------------*/
{
  Linked_list* list;
  Link* s;

  /* syntax check */

  if (!_before && !_after) return 0;
  if (!_before) _before = listr_prev(_after);
  if (!_after) _after   = listr_next(_before);

  if (_before) list = listr_father(_before);
  else list = listr_father(_after);

  s = (Link*) list_malloc (_size);

  if (_before) listr_set_next(_before, s);
  else listr_set_first(list, s);

  if (_after) listr_set_prev(_after, s);
  else listr_set_last(list, s);

  listr_set_prev   (s, _before);
  listr_set_next   (s, _after);
  listr_set_father (s, list);

  return (s);
}


void listr_remove_entry (Link* _entry)    {
  Linked_list* list = listr_father(_entry);
  if (!list) return;

  Link* prev = listr_prev(_entry);
  Link* next = listr_next(_entry);

  if (prev) listr_set_next (prev, next);
  else listr_set_first (list, next);

  if (next) listr_set_prev (next, prev);
  else listr_set_last (list, prev);
  free (_entry);
}

void listr_move_entry (Link* _entry, Link* _before, Link* _after) {
  listr_transfer_entry (_entry, listr_father(_entry), _before, _after);
}

void listr_disconnect_entry (Link* _entry)  {
  Linked_list* list = listr_father(_entry);
  if (!list) return;
  Link* prev = listr_prev(_entry);
  Link* next = listr_next(_entry);
  if (prev) listr_set_next (prev, next);
  else listr_set_first (list, next);
  if (next) listr_set_prev (next, prev);
  else listr_set_last (list, prev);
  _entry->father = 0;
}

void listr_transfer_entry (Link* _entry, Linked_list* _list,
                           Link* _before, Link* _after)
{
  /* syntax check */
  if (_before)  {
    if (_list != listr_father (_before)) return;
    if (_before == _entry) return;
  }
  if (_after)  {
    if (_list != listr_father(_after)) return;
    if (_after == _entry) return;
  }
  if (_before && _after)  {
    if (listr_next(_before) != _after ||
        _before != listr_prev(_after)) return;
  }

  listr_disconnect_entry (_entry);

  if (!_before && !_after) _after = listr_first(_list);
  if (!_before && _after) _before = listr_prev(_after);
  if (!_after && _before) _after  = listr_next(_before);

  //Linked_list* list = listr_father(_entry);

  /* Now put entry back. */

  if (_before) listr_set_next (_before, _entry);
  else listr_set_first (_list, _entry);

  if (_after) listr_set_prev (_after, _entry);
  else listr_set_last (_list, _entry);

  listr_set_next   (_entry, _after);
  listr_set_prev   (_entry, _before);
  listr_set_father (_entry, _list);
}


/*---------------------------------------------------------------------------*/
void listr_transfer (Linked_list* _from, Linked_list* _to)    {
  Link* l = listr_first (_from);
  if (l)  {
    if (_to->first) return;

    while (l)    {
      listr_set_father (l, _to);
      l = listr_next(l);
    }
    *_to = *_from;
    _from->first = _from->last = 0;
  }
}

void listr_convert (Linked_list* list)  {
  long i;
  if ((i = (long)list->first))  {
    i += (long)list;
    list->first = (Link*)i;
  }
  if ((i = (long)list->last))  {
    i += (long)list;
    list->last = (Link*)i;
  }
  for(Link* s=list->first; s; s = s->next)  {
    if ((i = (long)s->prev))    {
      i += (long)s;
      s->prev = (Link*)i;
    }
    if ((i = (long)s->next))  {
      i += (long)s;
      s->next = (Link*)i;
    }
    if ((i = (long)s->father))    {
      i += (long)s;
      s->father = (Linked_list*)i;
    }
  }
}

Link* listr_first (Linked_list* list) {
  return (list->first?(Link*)((long) list + (long) list->first):0);
}

Link* listr_last (Linked_list* list)  {
  return (list->last?(Link*)((long) list + (long) list->last):0);
}

Link* listr_prev (Link* entry)  {
  return (entry->prev?(Link*)((long) entry + (long) entry->prev):0);
}

Link* listr_next (Link* entry)    {
  return (entry->next?(Link*)((long) entry + (long) entry->next):0);
}

Linked_list* listr_father (Link* entry)   {
  return (entry->father?(Linked_list*)((long) entry + (long) entry->father):0);
}

void listr_set_first (Linked_list* list, Link* entry)   {
  list->first = (Link*)(entry?((long) entry - (long) list):0);
}

void listr_set_last (Linked_list* list, Link* entry)    {
  list->last = (Link*)(entry?((long) entry - (long) list):0);
}

void listr_set_next (Link* entry, Link* next)   {
  entry->next = (Link*)(next?((long) next - (long) entry):0);
}

void listr_set_prev (Link* entry, Link* prev)   {
  entry->prev = (Link*)(prev?((long) prev - (long) entry):0);
}

void listr_set_father ( Link* entry, Linked_list* father) {
  entry->father = (Linked_list*)(father?((long)father - (long)entry):0);
}

char* listr_malloc (int _size)  {
  char* ptr;
  if ((ptr = (char*) malloc (_size))) return (ptr);
  exit(1);
}
