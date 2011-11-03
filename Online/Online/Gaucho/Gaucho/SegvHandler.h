/*
 * SegvHandler.h
 *
 *  Created on: Oct 10, 2011
 *      Author: Beat Jost
 */
#include <signal.h>

#ifndef SEGVHANDLER_H_
#define SEGVHANDLER_H_
class SegvHandler
{
 private:
  SegvHandler();
  ~SegvHandler();
 public:
  struct sigaction act;
  struct sigaction oldact;
  static SegvHandler& instance();
};

#endif /* SEGVHANDLER_H_ */
