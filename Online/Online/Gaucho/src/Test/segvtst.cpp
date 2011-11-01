/*
 * segvtst.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: Beat Jost
 */

#include "Gaucho/SegvHandler.h"
int main(int , char **)
{
  new SegvHandler();
  M_SegvHandler->oldact = M_SegvHandler->oldact;
  int *p=(int *)7;

  int i;
  i =*p;
  return 0;
}
