/*
** DOUBLE LINKED QUEUE definition
*/
#ifndef __QUE_DOUBLELINKEDQUEUE_H__
#define __QUE_DOUBLELINKEDQUEUE_H__

#include "RTL/que.h"

/*
 *    namespace RTL
 */
namespace RTL  {
  /**@class DoubleLinkedQueue DoubleLinkedQueue.h RTL/DoubleLinkedQueue.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  template <class T> class DoubleLinkedQueue  {
  private:
    /// Queue head
    qentry_t* m_head;
    /// Park queue
    qentry_t* m_park;
    /// Member offset
    int m_offset;
  public:
    /// Initializing constructor
    explicit DoubleLinkedQueue(qentry_t* h, int o=0) 
      : m_head(h), m_park(new qentry_t(0,0)), m_offset(o) {}
    /// Default destructor
    ~DoubleLinkedQueue()  {
      qentry_t *e;
      for (int sc=remqhi(m_park,&e);lib_rtl_queue_success(sc);sc=remqhi(m_park,&e))
        insqhi (e, m_head);
      delete m_park;
    }
    /// Get next element in the queue
    T* get()  {
      qentry_t* e=0;
      int sc = remqhi(m_head,&e);
      if ( lib_rtl_queue_success(sc) )  {
        insqhi(e, m_park);
        return (T*)add_ptr(e, m_offset);
      }
      return 0;
    }
  };
}
#endif // __QUE_DOUBLELINKEDQUEUE_H__
