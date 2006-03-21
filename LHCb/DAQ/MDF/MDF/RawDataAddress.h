// $Id: RawDataAddress.h,v 1.2 2006-03-21 07:55:32 frankb Exp $
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
    /// Data member indicating the size of the event
    unsigned int   m_size;
    /// Event type identifier
    unsigned char  m_evType;
    /// Trigger mask used for event selection
    unsigned int   m_trMask[4];
    /// Pointer to vector of raw banks
    const Banks*   m_banks;
  public:
    /// Dummy constructor
    RawDataAddress() : GenericAddress(), m_size(0), m_evType(0), m_banks(0)  {
      m_trMask[0]=m_trMask[1]=m_trMask[2]=m_trMask[3] = 0;
      m_par[1] = "0 ";
    }
    /// Copy Constructor
    RawDataAddress( const RawDataAddress& c)
    : GenericAddress(c), m_size(c.m_size), m_evType(c.m_evType), m_banks(c.m_banks)  {
      m_trMask[0] = c.m_trMask[0];
      m_trMask[1] = c.m_trMask[1];
      m_trMask[2] = c.m_trMask[2];
      m_trMask[3] = c.m_trMask[3];
    }
    /// Standard Constructor
    RawDataAddress( long svc,
                    const CLID& clid,
                    const std::string& p1="", 
                    const std::string& p2="",
                    unsigned long ip1=0,
                    unsigned long ip2=0)  
    : GenericAddress(svc,clid,p1,p2,ip1,ip2), m_size(0), m_evType(0), m_banks(0)  {
      m_trMask[0]=m_trMask[1]=m_trMask[2]=m_trMask[3] = 0;
    }
    /// Standard Destructor
    virtual ~RawDataAddress()              {                      }
    /// Set pointer to rawevent banks
    void setBanks(const Banks* b)          { m_banks = b;         }
    /// Access to raw event banks (is set)
    const Banks* banks()  const            { return m_banks;      }
    /// Accessor: event size
    unsigned int  size() const             { return m_size;       }
    /// Update event size
    void setSize(unsigned int val)         { m_size = val;        }
    /// Accessor: event type identifier
    unsigned char eventType() const        { return m_evType;     }
    /// Update the event type
    void setEventType(unsigned int val)    { m_evType = val;      }  
    /// Accessor: trigger mask
    const unsigned int* triggerMask() const{ return m_trMask;     }
    /// Update the trigger mask of the event
    void setTriggerMask(const unsigned int* mask){
      m_trMask[0] = mask[0];
      m_trMask[1] = mask[1];
      m_trMask[2] = mask[2];
      m_trMask[3] = mask[3];
    }
    unsigned long long int fileOffset() const {
      unsigned long long int giga_bytes  = par()[1][0]-'0';
      unsigned long long int local_bytes = ipar()[0];
      return GIGA_BYTE*giga_bytes + local_bytes;
    }
    void setFileOffset(unsigned long long int offset) {
      char txt[2] = { '0' + int(offset/GIGA_BYTE), 0};
      m_ipar[1] = int(offset%GIGA_BYTE);
      m_par[1] = txt;
    }
  };
}

#endif // MDF_RAWDATAADDRESS_H

