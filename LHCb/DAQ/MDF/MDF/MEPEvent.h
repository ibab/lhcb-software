// $Id: MEPEvent.h,v 1.6 2007-04-20 12:40:24 cattanem Exp $
//	====================================================================
//  MBMEvent.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef EVENT_MEPEVENT_H
#define EVENT_MEPEVENT_H

// Framework include files
#include "MDF/MEPMultiFragment.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MEPEvent MEPEvent.h MDF/MEPEvent.h
    *
    * Structure to describe the data content originating from
    * one tell1 board.
    *
    * Note: A valid construction is only possible with in place
    *       creation on a sufficiently large block of memory
    *       containing the entire fragment size.
    *
    * @author  M.Frank
    * @version 1.0
    *
    */
  class MEPEvent   {
  public:
    typedef MEPMultiFragment Fragment;
    unsigned int   m_size;
    char           m_frags[4];

  public:
    /// Defautl constructor
    MEPEvent() : m_size(0)               {                                    }
    /// Initializing constructor
    MEPEvent(int siz) : m_size(siz)      {                                    }
    /// Length of the fragment
    size_t               size()    const {  return m_size;                    }
    /// Set event ID of the fragment
    void setSize(unsigned int val)       {  m_size = val;                     }
    /// Access to first bank in the fragment
    const char*          start()   const {  return m_frags;                   }
    /// Access to end-iteration over bank
    const char*          end()     const {  return m_frags+m_size-1;          }
    /// Access to first bank in the fragment
    Fragment*            first()   const { return (Fragment*)m_frags;         }
    /// Access to end-iteration over bank
    const Fragment*      last()    const { return (Fragment*)end();           }
    /// Access to the next bank
    static Fragment* next(const Fragment* last)  {
      // MEP size excludes MEP header size; need to advance 2 shorts more !
      return (Fragment*)(last->start()+last->size());
    }
    /// Object size ithout variable buffer
    static size_t sizeOf()  {  return sizeof(MEPEvent)-4*sizeof(char);     }
  };
}
#endif // EVENT_MEPEVENT_H
