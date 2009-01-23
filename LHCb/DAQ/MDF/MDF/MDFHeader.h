// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MDFHeader.h,v 1.9 2009-01-23 13:12:16 frankb Exp $
#ifndef EVENT_MDFHEADER
#define EVENT_MDFHEADER

// Framework include files
#include "GaudiKernel/Kernel.h"
#include <stdexcept>
#define DAQ_ERR_BANK_VERSION 0
#define DAQ_STATUS_BANK      16
#define DAQ_PROCERR_HEADER   32
#define DAQ_PROCERR_BANK     33
#define DAQ_FILEID_BANK      255

#ifdef _WIN32
#pragma pack(push, mdfheader_aligment, 1)
#define MDFHEADER_ALIGNED(x) x
#else
#define MDFHEADER_ALIGNED(x) x __attribute__((__packed__))
#endif

/*
 *   LHCb namespace
 */
namespace LHCb    {

  /** @struct MDFHeader  MDFHeader.h  MDF/MDFHeader.h
    *
    * Structure describing the header structure preceding each 
    * event buffer in MDF files.
    *
    * Known versions:
    * 0   : VELO testbeam  [early version]
    * 1   : RICH/MUON/OTR test beam
    * 2   : Empty specific header
    * 3   : New version (like 1, but with data type)
    *
    * Known data types:
    * 1 BODY_TYPE_BANKS
    * 2 BODY_TYPE_MEP
    *
    * Caution:
    * The data member need to be aligned in a way that the compiler
    * does not inject additional padding !
    *
    * @author  M.Frank
    * @version 1.0
    *
    */
  MDFHEADER_ALIGNED(class) MDFHeader  {
  public:
    enum { BODY_TYPE_BANKS=1, BODY_TYPE_MEP=2 };

    /// Data member indicating the size of the event
    unsigned int   m_size[3];
    /// Optional checksum over the event data (if 0, no checksum was calculated)
    unsigned int   m_checkSum;
    /// Identifier of the compression algorithm used to compress the data buffer
    unsigned char  m_compression;
    /// Header type: split into { version:4, length:4 } for possible future upgrade
    unsigned char  m_hdr;
    /// Data type
    unsigned char  m_dataType;
    /// Spare
    unsigned char  m_spare[1];

    MDFHEADER_ALIGNED(struct) HeaderTriggerMask  {
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
    MDFHEADER_ALIGNED(struct) Header0 {
      typedef long long int int_64_t;
      /// Event type identifier
      unsigned char  m_evType;
      /// High part of the 40 bit L0 trigger number
      unsigned char  m_trH;
      /// Low part of the 40 bit L0 trigger number
      unsigned int   m_trL;
      /// Trigger mask used for event selection
      unsigned int   m_trMask[4];
      Header0() : m_evType(0), m_trH(0), m_trL(0) {
        m_trMask[0] = m_trMask[1] = m_trMask[2] = m_trMask[3] = 0;
      }
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
    MDFHEADER_ALIGNED(struct) Header1  : public HeaderTriggerMask  {
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
    MDFHEADER_ALIGNED(struct) Header2  {
    };
    union SubHeader {
      void*   Pointer;
      Header0* H0;
      Header1* H1;
      Header2* H2;
      SubHeader(void* ptr)  { Pointer = ptr; }
    };
  public:
    static unsigned int sizeOf(int hdr_type)  {
      unsigned int len;
      switch(hdr_type)  {
        case 0:
          len = sizeof(MDFHeader)+sizeof(Header0)-2*sizeof(unsigned char);
          break;
        case 1:
          len = sizeof(MDFHeader)+sizeof(Header1);
          break;
        case 2:
          len = sizeof(MDFHeader);
          break;
        case 3:
          len = sizeof(MDFHeader)+sizeof(Header1);
          break;
        default:
          throw std::runtime_error("Unknown MDF header type!");
      }
      if ( (len%sizeof(int)) != 0 )  {
        throw std::runtime_error("Invalid MDF header length!");
      }
      return len;
    }
    /// Default constructor
    MDFHeader() : m_checkSum(0), m_compression(0), m_hdr(0), m_dataType(0)
    {
      m_size[0] = m_size[1] = m_size[2] = 0;
      m_spare[0] = 0;
      setSubheaderLength(0);
    }
    /// Default destructor
    ~MDFHeader()  {}
    /// Access record size
    unsigned int  recordSize()  const      { return m_size[0];                 }
    /// Accessor: event size
    unsigned int  size() const             
    { return m_size[0]-sizeOf(headerVersion());                                }
    /// Update event size
    void setSize(unsigned int val)         
    { m_size[0]=m_size[1]=m_size[2]=val+sizeOf(headerVersion());               }
    /// For checks: return 0th. size word
    unsigned int size0() const             { return m_size[0];                 }
    /// For checks: return 1rst. size word
    unsigned int size1() const             { return m_size[1];                 }
    /// For checks: return 2nd. size word
    unsigned int size2() const             { return m_size[2];                 }
    /// Accessor: checksum of the event data
    unsigned int  checkSum() const         { return m_checkSum;                }
    /// Update checksum of the event data
    void setChecksum(unsigned int val)     { m_checkSum = val;                 }
    /// Accessor: Identifier of the compression method
    unsigned char compression() const      { return m_compression;             }
    /// Update the identifier of the compression method
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
    /// Accessor: hdr field
    unsigned char hdr() const              { return m_hdr;                     }
    /// Update hdr field
    void setHdr(unsigned char val)         { m_hdr = val;                      }
    /// Accessor: event type identifier
    unsigned char dataType() const         { return m_dataType;                }
    /// Update the event type
    void setDataType(unsigned char val)    { m_dataType = val;                 }
    /// Set spare word
    void setSpare(unsigned char val)       { m_spare[0] = val;                 }
    /// Access to data payload (Header MUST be initialized)
    char* data() {  return ((char*)this)+sizeOf(headerVersion());              }
    /// Access to data payload (Header MUST be initialized)
    const char* data() const {  return ((char*)this)+sizeOf(headerVersion());  }
    
    /// Access to sub-headers
    SubHeader subHeader0()                 { return SubHeader(m_spare-1);      }
    SubHeader subHeader()                  { return SubHeader(m_spare+1);      }
  };
}    // End namespace LHCb

#undef MDFHEADER_ALIGNED
#ifdef _WIN32
#pragma pack(pop, mdfheader_aligment)
#endif
#endif // EVENT_MDFHEADER
