// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawEventDescriptor.h,v 1.6 2008-02-05 16:44:18 frankb Exp $
//  ====================================================================
//  MDFWriter.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef MDF_RAWEVENTDESCRIPTOR_H
#define MDF_RAWEVENTDESCRIPTOR_H

// Framework include file
#include "MDF/MEPFragment.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class RawEventHeader {
  public:
    /// Space for MEP_SINGLE_EVT structure
    long m_begin;
    int  m_evID;
    int  m_mepID;
    int  m_spare[2];
    /// RawEventHeader data
    int  m_NumFrag;
    int  m_ErrorMask;
    int  m_NumMiss;
    int  m_MissOffset;
    int  m_offsets[1];
    int  numberOfFragments() const          {  return m_NumFrag;          }
    int  offset(int which)   const          {  return m_offsets[which];   }
    int* offsets()                          {  return m_offsets;          }
    long dataStart()         const          {  return m_begin;            }
    int  eventID()           const          {  return m_evID;             }
    int  mepID()             const          {  return m_mepID;            }
    void setMEPID(int id)                   {  m_mepID = id;              }
    void setEventID(int id)                 {  m_evID = id;               }
    void setDataStart(int start)            {  m_begin = start;           }
    void setNumberOfFragments(size_t val)   {  m_NumFrag = val;           } 
    void setErrorMask(int val)              {  m_ErrorMask = val;         }
    void setNumberOfMissing(int val)        {  m_NumMiss = val;           }
    void setOffsetOfMissing(int val)        {  m_MissOffset = val;        }
    void setOffset(size_t frag, int off)    {  m_offsets[frag] = off;     }

    size_t size()  const  {  return sizeof(RawEventHeader)+(m_NumFrag-1)*sizeof(int); }
    static size_t size(size_t nfrag)   {  return sizeof(RawEventHeader)+(nfrag-1)*sizeof(int); }
    static size_t sizeOf()          {  return sizeof(RawEventHeader)-sizeof(int); }
  };

  class RawEventDescriptor   {
  public:
    typedef MEPFragment    _F;
    typedef RawEventHeader _H;
    unsigned int m_evType;
    int          m_PID;
    unsigned int m_TrMask[4];
    int          m_Size;
    char*        m_Ptr;
    char*        m_mepPtr;
    /// Accessor: event type identifier
    unsigned int eventType() const         { return m_evType;                  }
    /// Update the event type
    void setEventType(unsigned int val)    { m_evType = val;                   }  
    /// Accessor: event size
    unsigned int  size() const             { return m_Size;                    }
    /// Update event size
    void setSize(unsigned int val)         { m_Size=val;                       }
    /// Accessor: Partition ID
    int partitionID() const                { return m_PID;                     }
    /// Update Partition ID
    void setPartitionID( int pid )         { m_PID = pid;                      }
    /// Accessor: trigger mask
    const unsigned int* triggerMask() const{ return m_TrMask;                  }
    /// Update the trigger mask of the event
    void setTriggerMask(const unsigned int* mask)  {
      m_TrMask[0] = mask[0];
      m_TrMask[1] = mask[1];
      m_TrMask[2] = mask[2];
      m_TrMask[3] = mask[3];
    }
    const _H* header()      const { return (const _H*)m_Ptr;                   }
    void setHeader(void* val)     { m_Ptr = (char*)val;                        }
    const char* buffer()    const { return m_mepPtr;                           }
    void setMepBuffer(void* val)  { m_mepPtr = (char*)val;                     }
    int numberOfFragments() const { return header()->numberOfFragments();      }
    _F* fragment(int i) const  {   return (_F*)(m_mepPtr+header()->offset(i)); }
    int offset(int which)   const {  return header()->offset(which);           }
  };
}      // End namespace LHCb
#endif // MDF_RAWEVENTDESCRIPTOR_H
