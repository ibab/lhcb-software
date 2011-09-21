/*
 * BRTL_Lock.h
 *
 *  Created on: Jul 26, 2011
 *      Author: beat
 */

#ifndef BRTL_LOCK_H_
#define BRTL_LOCK_H_

#include <pthread.h>
#include <string>

class BRTLLock
{
public:
  pthread_mutex_t m_lid;
  std::string m_name;
  BRTLLock(void);
  ~BRTLLock(void);
  int lockMutex();
  int unlockMutex();
};
#endif /* BRTL_LOCK_H_ */
