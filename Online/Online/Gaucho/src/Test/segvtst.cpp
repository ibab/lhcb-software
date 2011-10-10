/*
 * segvtst.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: Beat Jost
 */

#include "../Gaucho/SegvHandler.h"
int main(int argc, char *argv[])
{
  SegvHandler *h = new SegvHandler();
  int *p=(int *)7;

  int i;
  i =*p;
  return 0;
}
