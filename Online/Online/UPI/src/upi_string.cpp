/*
        upic_STRING.C
         Created           :  6-NOV-1989 by 
*/
#include "SCR/lists.h"
#include <cstring>
#include <cstdlib>
//---------------------------------------------------------------------------
char** upic_build_string_list (char* addr, int elems, int elem_size);
void upic_delete_string_list (char** s, int elems);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
char** upic_build_string_list (char* addr, int elems, int elem_size)  {
  char** p = (char**) list_malloc (elems * sizeof (char*));
  char** q = p;
  for (int i=0; i<elems; i++, p++, addr += elem_size)  {
    *p = (char*) list_malloc (elem_size+1);
    strncpy (*p, addr, elem_size);
    (*p)[elem_size] = '\0';
  }
  return (q);
} 

//---------------------------------------------------------------------------
void upic_delete_string_list (char** s, int elems)
//---------------------------------------------------------------------------
{
  int i;

  if (s)
  {  
    for (i=0; i<elems; i++, s++)
    {
      if (*s) free (*s);
    }
    free (s);
  }
} 

