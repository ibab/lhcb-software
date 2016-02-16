/*
 * newnew.h
 *
 *  Created on: Feb 10, 2016
 *      Author: beat
 */
//#include "string.h"
void *operator new(size_t s)
{
  void *t = malloc(s);
  if (t == 0)
  {
    printf("New Operator... Cannot allocate memory of size %ld\n",s);
  }
  memset(t,0,s);
  return t;
}
