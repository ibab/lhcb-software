// $Id: InstanceCounter.h,v 1.1 2002-07-18 05:50:31 cattanem Exp $
#ifndef KERNEL_INSTANCECOUNTER_H 
#define KERNEL_INSTANCECOUNTER_H 1

// Include files
#include "GaudiKernel/System.h"
#include <iostream>

/** @class InstanceCounter InstanceCounter.h Kernel/InstanceCounter.h
 *  Static class used to instrument constructors and destructors to search
 *  for memory leaks
 *
 *  @author Markus Frank
 *  @date   2002-07-17
 */

template <class T> class InstanceCounter    {
  long m_count;
public:
  long increment()  { return ++m_count; }
  long decrement()  { return --m_count; }
  long count()const { return   m_count; }
  InstanceCounter() : m_count(0)   {
  }
  virtual ~InstanceCounter()   {
    std::cout << "Number of objects of type: "
              << System::typeinfoName(typeid(T))
              << " created, but not destroyed:" 
              << m_count
              << std::endl;
  }
};
#endif // KERNEL_INSTANCECOUNTER_H
