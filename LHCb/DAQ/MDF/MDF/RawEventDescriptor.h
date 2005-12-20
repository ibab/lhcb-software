// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawEventDescriptor.h,v 1.1 2005-12-20 16:33:38 frankb Exp $
//	====================================================================
//  MDFWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RAWEVENTDESCRIPTOR_H
#define MDF_RAWEVENTDESCRIPTOR_H

// Framework include file
#include "MDF/MEPFragment.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class RawEventHeader  {
  public:
    int m_NumFrag;
    int m_ErrorMask;
    int m_NumMiss;
    int m_MissOffset;
    int m_offsets[1];
    int numberOfFragments() const   {  return m_NumFrag;         }
    int offset(int which)   const   {  return m_offsets[which];  }
    size_t sizeOf()  const  {  return sizeof(RawEventHeader)+(m_NumFrag-1)*sizeof(int); }
  };

  class RawEventDescriptor   {
  public:
    typedef MEPFragment    _F;
    typedef RawEventHeader _H;
    int	         m_evType;
    int          m_PID;
    unsigned int m_TrMask[4];
    int          m_Size;
    char*        m_Ptr;
    char*        m_mepPtr;
    const _H* header()      const { return (const _H*)m_Ptr;                   }
    const char* buffer()    const { return m_mepPtr;                           }
    int numberOfFragments() const { return header()->numberOfFragments();      }
    _F* fragment(int i) const  {   return (_F*)(m_mepPtr+header()->offset(i)); }
  };
}      // End namespace LHCb
#endif // MDF_RAWEVENTDESCRIPTOR_H
