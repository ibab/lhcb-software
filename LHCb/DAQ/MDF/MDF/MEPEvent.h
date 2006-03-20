// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MEPEvent.h,v 1.2 2006-03-20 15:17:14 niko Exp $
//	====================================================================
//  MBMEvent.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef EVENT_MEPEVENT_H
#define EVENT_MEPEVENT_H

#include "MDF/MEPFragment.h"
namespace LHCb    {

  class MultiEventPacket  {
  public:
    unsigned int   m_L0ID;
    unsigned short m_numEvent;
    unsigned short m_size;
    unsigned int   m_partitionID;
    MEPFragment    m_fragments[1];
    size_t size()  const   {      return m_size;      }
  };

  class MultiEvent  {
  public:
    struct MepPacketWrapper  {
      int              m_bufferStart;
      int              m_sourceID;
      MultiEventPacket m_packet;
      size_t           sizeOf()   const
      { return m_packet.size() + sizeof(MepPacketWrapper) - sizeof(MultiEventPacket); }
    };
    int               m_refCount;
    int               m_packingFactor;
    int               m_bufferStart;
    int               m_bufferLength;
    int               m_numMeps;
    MepPacketWrapper  m_meps[1];
    const char* start()              const {  return (const char*)m_meps;    }
    const char* end()                const {  return start()+m_bufferLength; }
    const MepPacketWrapper* first()  const {  return &m_meps[0];             }
    const MepPacketWrapper* next(const MepPacketWrapper* prev)  const   {
      const char* p = ((const char*)prev) + prev->sizeOf();
      return (MepPacketWrapper*)(p-start()>=m_bufferLength ? 0 : p);
    }
  };


}
// Framework include files
#include "MDF/MEPMultiFragment.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MEPFragment MEPFragment.h Event/MEPFragment.h
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
    MEPEvent() : m_size(0)               {                                    }
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
    Fragment* next(const Fragment* last) const {
      // MEP size excludes MEP header size; need to advance 2 shorts more !
      return (Fragment*)(last->start()+last->size()-12);
    }
    /// Object size ithout variable buffer
    static size_t sizeOf()  {  return sizeof(MEPEvent)-4*sizeof(char);     }
  };
}
#endif // EVENT_MEPEVENT_H
