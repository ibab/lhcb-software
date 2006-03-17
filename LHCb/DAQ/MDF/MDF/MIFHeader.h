// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MIFHeader.h,v 1.1 2006-03-17 17:24:47 frankb Exp $
#ifndef EVENT_MIFHEADER
#define EVENT_MIFHEADER

// Framework include files
#include "GaudiKernel/Kernel.h"

/*
 *   LHCb namespace
 */
namespace LHCb    {

  /** @struct MIFHeader  MIFHeader.h  MIF/MIFHeader.h
    *
    * Structure describing an index entry to identify events in MDF files.
    *
    * Caution:
    * The data member need to be aligned in a way that the compiler
    * does not inject additional padding !
    *
    * @author  M.Frank
    * @version 1.0
    *
    */
  class MIFHeader  {
  public:
    typedef unsigned long long int uint_64_t;
    /// Data member indicating the file id
    unsigned short     m_fid;
    /// MIF type identifier
    unsigned char      m_type;
    /// MIF size without header (=data size)
    unsigned char      m_size;
    unsigned char      m_data[1];
    struct Event {
      /// Absolute offset of the event in the data file
      uint_64_t          m_offset;
      /// Trigger mask used for event selection
      unsigned int       m_trMask[4];
      Event() : m_offset(0) { m_trMask[0] = m_trMask[1] = m_trMask[2] = m_trMask[3] = 0; }
      void setOffset(uint_64_t value) { m_offset = value; }
      uint_64_t offset() const        { return m_offset;  }
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
  public:
    enum { MIF_EVENT, MIF_FID };
    template <class T> static 
    std::pair<MIFHeader*,T*> create(void* memory, unsigned short fid, unsigned char typ)  {
      if ( sizeof(T) < 256 )  {
        MIFHeader* hdr = new (memory) MIFHeader(fid, typ, sizeof(T));
        return std::make_pair(hdr, new(hdr->m_data) T());
      }
      return std::make_pair((MIFHeader*)0, (T*)0);
    }
    static MIFHeader* create(void* memory, const std::string& fname, unsigned short fid)  {
      size_t siz  = fname.length();
      if ( siz < 256 )  {
        MIFHeader* hdr = new (memory) MIFHeader(fid, MIF_FID, siz);
        ::memcpy(hdr->m_data, fname.data(), siz);
        if ( siz%sizeof(int) == 0 )  {
          hdr->m_data[siz] = 0;
          return hdr;
        }
        ::memset(hdr->m_data+siz,0,siz%sizeof(int));
        return hdr;
      }
      return 0;
    }

    /// Default constructor
    MIFHeader() : m_fid(0), m_type(0), m_size(0)    {
    }
    /// Initializing constructor
    MIFHeader(unsigned short fid, unsigned char typ, unsigned char siz) 
      : m_fid(fid), m_type(typ), m_size(siz) 
    {
    }
    /// Default destructor
    ~MIFHeader()  {}
    /// Accessor: MIF record type
    unsigned int  type() const             { return m_type;                    }
    /// Update MIF record type
    void setType(unsigned char val)        { m_type = val;                     }
    /// Accessor: MIF data size
    size_t size() const             { return m_size;                    }
    /// Update MIF data size
    void setSize(unsigned int val)         { m_size = val;                     }
    size_t totalSize()  const              { return m_size+sizeof(MIFHeader)-sizeof(m_data); }
    template<class T> T* data() const  {  return (T*)m_data; }
  };
}    // End namespace LHCb
#endif // EVENT_MIFHEADER
