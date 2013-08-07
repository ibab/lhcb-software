// $Id: RawDataAddress.h,v 1.9 2007-12-14 11:42:23 frankb Exp $
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
  public:
    static const size_t GIGA_BYTE = 1073741824;   // == (1024*1024*1024)
    typedef std::vector<RawBank*> Banks;

  protected:
    /// Pointer to vector of raw bank
    std::pair<char*,int> m_data;
    /// Pointer to vector of raw banks
    const Banks*         m_banks;
    /// Flag with recipe to access transient data
    int                  m_type;

  private:
    /// Assigment opertor is not allowed!
    RawDataAddress& operator=(const RawDataAddress& ) {
      return *this; 
    }
  public:
    enum { NO_TYPE, DATA_TYPE, BANK_TYPE, MEP_TYPE, TAE_TYPE };

    /// Dummy constructor
    RawDataAddress() : GenericAddress(), m_data(0,0),m_banks(0), m_type(NO_TYPE)  {
      m_par[1] = "0 ";
    }
    /// Copy Constructor
    RawDataAddress(const RawDataAddress& c) : GenericAddress(c),
      m_data(c.m_data), m_banks(c.m_banks), m_type(c.m_type)  {
    }
    /// Standard Constructor
    RawDataAddress( long svc,
                    const CLID& clid,
                    const std::string& p1="", 
                    const std::string& p2="",
                    unsigned long ip1=0,
                    unsigned long ip2=0)  
    : GenericAddress(svc,clid,p1,p2,ip1,ip2), m_data(0,0), m_banks(0), m_type(NO_TYPE)
    {    }
    /// Standard Destructor
    virtual ~RawDataAddress()              {                      }
    /// Pointer to raw buffer
    std::pair<char*,int> data()  const     { return m_data;       }
    /// Set data buffer
    void setData(const std::pair<char*,int>& val) { m_data = val; }
    /// Access to raw event banks (is set)
    const Banks* banks()  const            { return m_banks;      }
    /// Set pointer to rawevent banks
    void setBanks(const Banks* b)          { m_banks = b;         }
    /// Pointer to raw buffer
    int type()  const                      { return m_type;       }
    /// Set data buffer
    void setType(int val)                  { m_type = val;        }
    /// Access file offset
    unsigned long long int fileOffset() const {
      unsigned long long int giga_bytes  = par()[1][0]-'0';
      unsigned long long int local_bytes = ipar()[1];
      return GIGA_BYTE*giga_bytes + local_bytes;
    }
    /// Update file offset
    void setFileOffset(unsigned long long int offset) {
      char txt[2] = { char('0' + int(offset/GIGA_BYTE)), 0};
      m_ipar[1] = int(offset%GIGA_BYTE);
      m_par[1] = txt;
    }
  };
}
#endif // MDF_RAWDATAADDRESS_H
