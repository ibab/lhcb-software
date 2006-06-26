// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MDFHeader.h,v 1.3 2006-06-26 08:37:16 frankb Exp $
#ifndef EVENT_MDFHEADER
#define EVENT_MDFHEADER

// Framework include files
#include "GaudiKernel/Kernel.h"
#include <stdexcept>

#define DAQ_STATUS_BANK 16

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
    /// Header type: split into { version:4, length:4 } for possible future upgrade
    unsigned char  m_hdr;
    /// Opaque data buffer
    unsigned char  m_data[2];

    struct HeaderTriggerMask  {
      /// Trigger mask used for event selection
      unsigned int   m_trMask[4];
      HeaderTriggerMask() {
        m_trMask[0] = m_trMask[1] = m_trMask[2] = m_trMask[3] = 0;
      }
      /// Accessor: Number of bits in the trigger mask
      unsigned int  maskBits() const         { return sizeof(m_trMask)*8;        }
      /// Accessor: trigger mask
      const unsigned int* triggerMask() const{ return m_trMask;                  }
      /// Update the trigger mask of the event
      void setTriggerMask(const unsigned int* mask){
        m_trMask[0] = mask[0];
        m_trMask[1] = mask[1];
        m_trMask[2] = mask[2];
        m_trMask[3] = mask[3];
      }
    };
    struct Header0 : public HeaderTriggerMask {
      typedef long long int int_64_t;
      /// Event type identifier
      unsigned char  m_evType;
      /// High part of the 40 bit L0 trigger number
      unsigned char  m_trH;
      /// Low part of the 40 bit L0 trigger number
      unsigned int   m_trL;
      Header0() : m_evType(0), m_trH(0), m_trL(0) {}
      /// Accessor: event type identifier
      unsigned char eventType() const        { return m_evType;                  }
      /// Update the event type
      void setEventType(unsigned int val)    { m_evType = val;                   }  
      /// Accessor: L0 trigger number of the event  
      long long     triggerNumber() const    { return (int_64_t(m_trH)<<32)+m_trL;}
      /// Update the L0 trigger number of the event
      void setTriggerNumber(int_64_t val)    {
        m_trH = char(0xFF&(val>>32));
        m_trL = (unsigned int)(0xFFFFFFFFLL&(val&0xFFFFFFFFLL));
      }
    };
    struct Header1  : public HeaderTriggerMask  {
      /// Run number
      unsigned int   m_runNumber;
      /// Orbit counter
      unsigned int   m_orbitCount;
      /// Bunch identifier
      unsigned int   m_bunchID;
      /// Set run number
      void setRunNumber(unsigned int runno)   { m_runNumber  = runno;   }
      /// Set orbit counter
      void setOrbitNumber(unsigned int orbno) { m_orbitCount = orbno;   }
      /// Set bunch identifier
      void setBunchID(unsigned int bid)       { m_bunchID    = bid;     }
      /// Access run number
      unsigned int runNumber() const          { return m_runNumber;     }
      /// Access run number
      unsigned int orbitNumber() const        { return m_orbitCount;    }
      /// Access run number
      unsigned int bunchID()   const          { return m_bunchID;       }
    };
    union SubHeader {
      void*   Pointer;
      Header0* H0;
      Header1* H1;
      SubHeader(void* ptr)  { Pointer = ptr; }
    };
  public:
    static unsigned int rawSize()  {
      return sizeof(MDFHeader)-2*sizeof(unsigned char);
    }
    static unsigned int sizeOf(int hdr_type)  {
      unsigned int len;
      switch(hdr_type)  {
        case 0:
          len = sizeof(MDFHeader)+sizeof(Header0)-2*sizeof(unsigned char);
        case 1:
          len = sizeof(MDFHeader)+sizeof(Header1)-2*sizeof(unsigned char);
        default:
          throw std::runtime_error("Unknown MDF header type!");
      }
      if ( (len%sizeof(int)) != 0 )  {
        throw std::runtime_error("Invalid MDF header length!");
      }
      return len;
    }
    /// Default constructor
    MDFHeader() : m_checkSum(0), m_compression(0), m_hdr(0)
    {
      m_size[0] = m_size[1] = m_size[2] = 0;
      setSubheaderLength(0);
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
    /// Accessor: length of the event header
    unsigned int subheaderLength() const   { return (m_hdr&0x0F)*sizeof(int);  }
    /// Update the length of the event header
    void setSubheaderLength(unsigned int l)  {
       l = l%sizeof(int) ? (l/sizeof(int)) + 1 : l/sizeof(int);
       m_hdr = (0xF0&m_hdr) + (0x0F&l);
    }
    /// Accessor: version of the event header
    unsigned int  headerVersion() const    { return m_hdr>>4;                  }
    /// Update the version of the event header
    void setHeaderVersion(unsigned int vsn){ m_hdr = ((vsn<<4)+(m_hdr&0xF))&0xFF;}
    SubHeader subHeader()                  { return SubHeader(&m_data[0]);     }
  };
}    // End namespace LHCb
#endif // EVENT_MDFHEADER
