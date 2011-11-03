#ifndef _FRAMEWORK_LOCK_H
#define _FRAMEWORK_LOCK_H

/*
 *   Framework namespace declaration
 */
namespace Framework  {

  /**@class Lock Lock.h Framework/Lock.h
    */
  template <class T> class Lock  {
    /// Execution mode (inverted or not
    bool           m_mode;
    /// Lock status
    int            m_status;
    /// Lock object
    T*             m_id;
  public:
    /// Initializing constructor for inverted lock
    explicit Lock(T* id, bool inverted=false)  : m_mode(inverted), m_id(id) {
      m_status = m_mode ? m_id->unlock() : m_id->lock();
    }
    /// Standard destructor
    ~Lock() {
      m_status = m_mode ? m_id->lock() : m_id->unlock();
      if ( !m_status )  {
	// throw exception ?
      }
    }
    /// Validity check
    operator int ()   {
      return 1==(1&m_status);
    }
    /// Access to object status
    int status()      const   {
      return m_status;
    }
  };
}

#endif // _FRAMEWORK_LOCK_H
