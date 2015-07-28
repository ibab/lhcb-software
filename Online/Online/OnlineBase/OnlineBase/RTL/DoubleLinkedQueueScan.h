/*
** DOUBLE LINKED QUEUE definition
*/
#ifndef __QUE_DOUBLELINKEDQUEUESCAN_H__
#define __QUE_DOUBLELINKEDQUEUESCAN_H__

#include "RTL/que.h"

/*
 *    namespace RTL
 */
namespace RTL  {
  /**@class DoubleLinkedQueueScan DoubleLinkedQueueScan.h RTL/DoubleLinkedQueueScan.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  template <class T> class DoubleLinkedQueueScan  {
  private:
    /// Queue head
    qentry_t* m_head;
    /// Park queue
    qentry_t* m_next;
    /// Member offset
    int m_offset;
  public:
    /// Initializing constructor
    explicit DoubleLinkedQueueScan(qentry_t* h, int o=0) : m_head(h), m_next(h), m_offset(o) {
      m_next = (qentry_t*)(((char*)m_next)+(((long)m_next->next)));
    }
    /// Default destructor
    ~DoubleLinkedQueueScan()  {}
    /// Get next element in the queue
    T* get()  {
      if ( m_next ) {
        qentry_t* e = m_next;
        if ( e != m_head ) {
          m_next = (qentry_t*)(((char*)m_next)+(((long)m_next->next)));
          return (T*)add_ptr(e, m_offset);
        }
        m_next = 0;
      }
      return 0;
    }
  };
}
#endif // __QUE_DOUBLELINKEDQUEUESCAN_H__
