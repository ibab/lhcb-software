// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MDFHeader.h,v 1.1 2005-12-20 16:33:38 frankb Exp $
#ifndef EVENT_MDFHEADER
#define EVENT_MDFHEADER

// Framework include files
#include "GaudiKernel/Kernel.h"

/*
 *   LHCb namespace
 */
namespace LHCb    {

  /** @struct MDFHeader  MDFHeader.h  MDF/MDFHeader.h
    *
    * Structure describing the header structure preceding each 
    * event buffer in MDF files.
    *
    * Caution:
    * The data member need to be aligned in a way that the compiler
    * does not inject additional padding !
    *
    * @author  M.Frank
    * @version 1.0
    *
    */
  class MDFHeader  {
  public:
    /// Data member indicating the size of the event
    unsigned int   m_size[3];
    /// Optional checksum over the event data (if 0, no checksum was calculated)
    unsigned int   m_checkSum;
    /// Identifier of the compression algorithm used to compress the data buffer
    unsigned char  m_compression;
    /// Event type identifier
    unsigned char  m_evType;
    /// Header type: split into { version:4, length:4 } for possible future upgrade
    unsigned char  m_hdr;
    /// High part of the 40 bit L0 trigger number
    unsigned char  m_trH;
    /// Low part of the 40 bit L0 trigger number
    unsigned int   m_trL;
    /// Trigger mask used for event selection
    unsigned int   m_trMask[4];

  public:
    /// Default constructor
    MDFHeader() : m_checkSum(0), m_compression(0), m_evType(0), m_hdr(0), m_trH(0), m_trL(0)
    {
      m_trMask[0] = m_trMask[1] = m_trMask[2] = m_trMask[3] = 0;
      m_size[0] = m_size[1] = m_size[2] = 0;
      setHeaderLength(sizeof(MDFHeader));
    }
    /// Default destructor
    ~MDFHeader()  {}
    /// Accessor: event size
    unsigned int  size() const             { return m_size[0];                 }
    /// Update event size
    void setSize(unsigned int val)         { m_size[0]=m_size[1]=m_size[2]=val;}
    /// Accessor: checksum of the event data
    unsigned int  checkSum() const         { return m_checkSum;                }
    /// Update checksum of the event data
    void setChecksum(unsigned int val)     { m_checkSum = val;                 }
    /// Accessor: Identifier of the compression algorithm
    unsigned char compression() const      { return m_compression;             }
    /// Update the identifier of the compression algorithm
    void setCompression(unsigned int val)  { m_compression = val;              }
    /// Accessor: event type identifier
    unsigned char eventType() const        { return m_evType;                  }
    /// Update the event type
    void setEventType(unsigned int val)    { m_evType = val;                   }  
    /// Accessor: length of the event header
    unsigned int  headerLength() const     { return m_hdr&0x0F;                }
    /// Update the length of the event header
    void setHeaderLength(unsigned int len) { m_hdr = (len&0x0F)+(m_hdr&0xF0);  }
    /// Accessor: version of the event header
    unsigned int  headerVersion() const    { return m_hdr>>4;                  }
    /// Update the version of the event header
    void setHeaderVersion(unsigned int vsn){ m_hdr = ((vsn<<4)+m_hdr)&0xFF;    }
    /// Accessor: Number of bits in the trigger mask
    unsigned int  maskBits() const         { return sizeof(m_trMask)*8;        }
    /// Accessor: L0 trigger number of the event  
    longlong      triggerNumber() const    { return (longlong(m_trH)<<32)+m_trL;}
    /// Update the L0 trigger number of the event
    void setTriggerNumber(longlong val)    {
      m_trH = char(0xFF&(val>>32));
      m_trL = (unsigned int)(0xFFFFFFFFLL&(val&0xFFFFFFFFLL));
    }
    /// Accessor: trigger mask
    const unsigned int* triggerMask() const{ return m_trMask;                  }
    /// Update the trigger mask of the event
    void setTriggerMask(unsigned int* mask){
      m_trMask[0] = mask[0];
      m_trMask[1] = mask[1];
      m_trMask[2] = mask[2];
      m_trMask[3] = mask[3];
    }
  };
}    // End namespace LHCb
#endif // EVENT_MDFHEADER
