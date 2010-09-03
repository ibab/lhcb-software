// $Id: MEPMultiFragment.h,v 1.8 2008-02-05 16:44:18 frankb Exp $
//====================================================================
//  MEPMultiFragment.h
//--------------------------------------------------------------------
//
//====================================================================
#ifndef MDF_MEPMULTIFRAGMENT_H
#define MDF_MEPMULTIFRAGMENT_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MEPMultiFragment.h,v 1.8 2008-02-05 16:44:18 frankb Exp $

// Framework include files
#include "MDF/MEPFragment.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class MEPFragment;

  /** @class MEPMultiFragment MEPMultiFragment.h Event/MEPMultiFragment.h
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
  class MEPMultiFragment  {
  protected:
    /// Bank definition
    typedef LHCb::MEPFragment Fragment;
    /// placeholder for IP header
    char m_ipheader[20];
    /// Event ID
    unsigned int   m_evID;
    /// Packing factor
    unsigned short m_numEvt;
    /// Fragment size (=size of all banks including header)
    unsigned short m_size;
    /// Partition identifier
    unsigned int   m_partitionID;
    /// First byte of event data
    char           m_frags[4];

  public:
    /// Default constructor
    MEPMultiFragment()
    : m_evID(0), m_numEvt(0), m_size(12), m_partitionID(0) {                }
    /// Initializing constructor
    MEPMultiFragment(unsigned int eid, unsigned short siz, int pID, int packing) 
    : m_evID(eid),m_numEvt((unsigned short)packing),m_size(siz),m_partitionID(pID){}
    /// Default destructor
    ~MEPMultiFragment()                  {                                    }
    /// Access to IP header
    char* ipHeader()                     {  return m_ipheader;                }
    /// Access event ID of the fragment
    unsigned int     partitionID() const {  return m_partitionID;             }
    /// Set event ID of the fragment
    void setPartitionID(unsigned int val){  m_partitionID = val;              }
    /// Access event ID of the fragment
    unsigned int         eventID() const {  return m_evID;                    }
    /// Set event ID of the fragment
    void setEventID(unsigned int val)    {  m_evID = val;                     }
    /// Header size (=12 bytes)
    static size_t hdrSize()              {  return 12;                        }
    /// Length of the fragment       (Note: 12 Bytes mep multifragment header!)
    size_t               size()    const {  return m_size-hdrSize();          }
    /// Set event ID of the fragment (Note: 12 Bytes mep multifragment header!)
    void setSize(unsigned short val)     {  m_size = val+hdrSize();           }
    /// Packing factor
    size_t               packing() const {  return m_numEvt;                  }
    /// Set packing factor
    void  setPacking(unsigned short val) {  m_numEvt = val;                   }
    /// Access to first bank in the fragment
    const char*          start()   const {  return m_frags;                   }
    /// Access to end-iteration over bank (Note: 12 Bytes mep multifragment header!)
    const char*          end()     const {  return m_frags+size();            } //-4*sizeof(char);}
    /// Access to first bank in the fragment
    Fragment*            first()   const { return (Fragment*)m_frags;         }
    /// Access to end-iteration over bank
    const Fragment*      last()    const { return (Fragment*)end();           }
    /// Access to the next bank
    static Fragment* next(const Fragment* last) {
      // MEP size excludes MEP header size; need to advance 2 shorts more !
      return (Fragment*)(last->start()+last->size());
    }
    /// Object size ithout variable buffer (Note: 12 Bytes mep multifragment header!)
    static size_t sizeOf()  {  return sizeof(MEPMultiFragment)-4*sizeof(char); }
  };
}      // end namespace LHCb
#endif // MDF_MEPMULTIFRAGMENT_H
