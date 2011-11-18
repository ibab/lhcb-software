/*
 * segvtst.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: Beat Jost
 */
#include "stdio.h"
#include "Gaucho/SegvHandler.h"
int main(int , char **)
{
  SegvHandler::instance();
//  M_SegvHandler->oldact = M_SegvHandler->oldact;
  int *p=(int *)7;

  int i;
  i =*p;
  printf("%d\n",i);
  return 0;
}
