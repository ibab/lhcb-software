// $Id: RawDataAddress.h,v 1.4 2006-06-26 08:37:16 frankb Exp $
#ifndef MDF_RAWDATAADDRESS_H
#define MDF_RAWDATAADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include <vector>

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declaration
  class RawBank;

  /** @class RawDataAddress RawDataAddress.h GaudiKernel/RawDataAddress.h
    *
    * Generic Transient Address fro MBM or RawData data.
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class RawDataAddress : public GenericAddress  {
    typedef std::vector<RawBank*> Banks;
    static const size_t GIGA_BYTE = 1073741824;   // == (1024*1024*1024)

  protected:
    /// Pointer to vector of raw banks
    const Banks*   m_banks;
    size_t         m_dataLen;
    const void*    m_data;
  public:
    /// Dummy constructor
    RawDataAddress() : GenericAddress(), m_banks(0), m_data(0), m_dataLen(0)  {
      m_par[1] = "0 ";
    }
    /// Copy Constructor
    RawDataAddress( const RawDataAddress& c)
    : GenericAddress(c), m_banks(c.m_banks), m_dataLen(c.m_dataLen), m_data(c.m_data)
    {
    }
    /// Standard Constructor
    RawDataAddress( long svc,
                    const CLID& clid,
                    const std::string& p1="", 
                    const std::string& p2="",
                    unsigned long ip1=0,
                    unsigned long ip2=0)  
    : GenericAddress(svc,clid,p1,p2,ip1,ip2), m_banks(0), m_data(0), m_dataLen(0)  {
    }
    /// Standard Destructor
    virtual ~RawDataAddress()              {                      }
    /// Set pointer to rawevent banks
    void setBanks(const Banks* b)          { m_banks = b;         }
    /// Access to raw event banks (is set)
    const Banks* banks()  const            { return m_banks;      }
    /// Pointer to raw buffer
    const void* data()  const              { return m_data;       }
    /// Update pointer to raw buffer
    void setData(const void* ptr)          { m_data = ptr;        }
    /// Length of raw data buffer
    size_t dataLength()  const             { return m_dataLen;    }
    /// Update length of raw data buffer
    void setDataLength(size_t len)         { m_dataLen = len;     }
    /// Access file offset
    unsigned long long int fileOffset() const {
      unsigned long long int giga_bytes  = par()[1][0]-'0';
      unsigned long long int local_bytes = ipar()[0];
      return GIGA_BYTE*giga_bytes + local_bytes;
    }
    /// Update file offset
    void setFileOffset(unsigned long long int offset) {
      char txt[2] = { '0' + int(offset/GIGA_BYTE), 0};
      m_ipar[1] = int(offset%GIGA_BYTE);
      m_par[1] = txt;
    }
  };
}

#endif // MDF_RAWDATAADDRESS_H
