
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef STTELL1Event_STTELL1Data_H
#define STTELL1Event_STTELL1Data_H 1

// Include files
#include "Event/STTELL1EventInfo.h"
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRefVector.h"

// Forward declarations

namespace LHCb 
{

  // Forward declarations
  
  // Class ID definition
  static const CLID CLID_STTELL1Data = 9010;
  
  // Namespace for locations in TDS
  namespace STTELL1DataLocation {
    static const std::string& ITFull = "Raw/IT/Full";
    static const std::string& TTFull = "Raw/TT/Full";
    static const std::string& ITPedestal = "Raw/IT/Pedestal";
    static const std::string& TTPedestal = "Raw/TT/Pedestal";
    static const std::string& ITProcFull = "Raw/IT/ProcFull";
    static const std::string& TTProcFull = "Raw/TT/ProcFull";
    static const std::string& ITPedSubADCs = "Raw/IT/PedSubADCs";
    static const std::string& TTPedSubADCs = "Raw/TT/PedSubADCs";
    static const std::string& ITSubPeds = "Raw/IT/SubPeds";
    static const std::string& TTSubPeds = "Raw/TT/SubPeds";
    static const std::string& ITLCMSADCs = "Raw/IT/LCMSADCs";
    static const std::string& TTLCMSADCs = "Raw/TT/LCMSADCs";
    static const std::string& ITEmulated = "Raw/IT/Emulated";
    static const std::string& TTEmulated = "Raw/TT/Emulated";
  }
  

  /** @class STTELL1Data STTELL1Data.h
   *
   * ADCs from a TELL1 board 
   *
   * @author Matthew Needham
   * created Thu Dec  6 16:53:50 2007
   * 
   */

  class STTELL1Data: public KeyedObject< int > 
  {
  public:

    /// typedef for KeyedContainer of STTELL1Data
    typedef KeyedContainer<STTELL1Data, Containers::HashMap> Container;
  
    /// 2-d data vector of signed int typedef
    typedef std::vector <  std::vector < signed int > > Data;
    /// Vector of STTELL1EventInfo typedef
    typedef SmartRefVector<LHCb::STTELL1EventInfo> Info;
  
    /// constructor with arguments
    STTELL1Data(const Data& data) : m_data(data) {}
  
    /// constructor with key
    STTELL1Data(const int numberOfTELL1) : KeyedObject<int>(numberOfTELL1),
                                           m_data(),
                                           m_header() {}
  
    /// Default Constructor
    STTELL1Data() : m_data(),
                    m_header() {}
  
    /// Default Destructor
    virtual ~STTELL1Data() {}
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// size of TELL1 data
    unsigned int dataSize() const;
  
    /// Returns the TELL1D id
    unsigned int TELL1ID() const;
  
    /// Returns the data of a single beetle
    const std::vector < signed int > & beetleData(unsigned char beetleID) const;
  
    /// Retrieve const  vector of raw data
    const Data& data() const;
  
    /// Retrieve  vector of raw data
    Data& data();
  
    /// Update  vector of raw data
    void setData(const Data& value);
  
    /// Retrieve const  vector of headers
    const Data& header() const;
  
    /// Update  vector of headers
    void setHeader(const Data& value);
  
    /// Retrieve (const)  Vector of 4 Event Info block (1 per PP)
    const SmartRefVector<LHCb::STTELL1EventInfo>& eventInfo() const;
  
    /// Update  Vector of 4 Event Info block (1 per PP)
    void setEventInfo(const SmartRefVector<LHCb::STTELL1EventInfo>& value);
  
    /// Add to  Vector of 4 Event Info block (1 per PP)
    void addToEventInfo(const SmartRef<LHCb::STTELL1EventInfo>& value);
  
    /// Att to (pointer)  Vector of 4 Event Info block (1 per PP)
    void addToEventInfo(const LHCb::STTELL1EventInfo* value);
  
    /// Remove from  Vector of 4 Event Info block (1 per PP)
    void removeFromEventInfo(const SmartRef<LHCb::STTELL1EventInfo>& value);
  
    /// Clear  Vector of 4 Event Info block (1 per PP)
    void clearEventInfo();
  
  
  #ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(STTELL1Data) == size ? 
               boost::singleton_pool<STTELL1Data, sizeof(STTELL1Data)>::malloc() :
               ::operator new(size) );
    }
  
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<STTELL1Data, sizeof(STTELL1Data)>::is_from(p) ?
      boost::singleton_pool<STTELL1Data, sizeof(STTELL1Data)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
  #endif
  protected:

  private:

    Data                                   m_data;      ///< vector of raw data
    Data                                   m_header;    ///< vector of headers
    SmartRefVector<LHCb::STTELL1EventInfo> m_eventInfo; ///< Vector of 4 Event Info block (1 per PP)
  
  }; // class STTELL1Data

  /// Definition of Keyed Container for STTELL1Data
  typedef KeyedContainer<STTELL1Data, Containers::HashMap> STTELL1Datas;
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& LHCb::STTELL1Data::clID() const
{
  return LHCb::STTELL1Data::classID();
}

inline const CLID& LHCb::STTELL1Data::classID()
{
  return CLID_STTELL1Data;
}

inline const LHCb::STTELL1Data::Data& LHCb::STTELL1Data::data() const 
{
  return m_data;
}

inline LHCb::STTELL1Data::Data& LHCb::STTELL1Data::data() 
{
  return m_data;
}

inline void LHCb::STTELL1Data::setData(const Data& value) 
{
  m_data = value;
}

inline const LHCb::STTELL1Data::Data& LHCb::STTELL1Data::header() const 
{
  return m_header;
}

inline void LHCb::STTELL1Data::setHeader(const Data& value) 
{
  m_header = value;
}

inline const SmartRefVector<LHCb::STTELL1EventInfo>& LHCb::STTELL1Data::eventInfo() const 
{
  return m_eventInfo;
}

inline void LHCb::STTELL1Data::setEventInfo(const SmartRefVector<LHCb::STTELL1EventInfo>& value) 
{
  m_eventInfo = value;
}

inline void LHCb::STTELL1Data::addToEventInfo(const SmartRef<LHCb::STTELL1EventInfo>& value) 
{
  m_eventInfo.push_back(value);
}

inline void LHCb::STTELL1Data::addToEventInfo(const LHCb::STTELL1EventInfo* value) 
{
  m_eventInfo.push_back(value);
}

inline void LHCb::STTELL1Data::removeFromEventInfo(const SmartRef<LHCb::STTELL1EventInfo>& value) 
{
  SmartRefVector<LHCb::STTELL1EventInfo>::iterator iter =
    std::remove(m_eventInfo.begin(), m_eventInfo.end(), value);
  m_eventInfo.erase(iter, m_eventInfo.end());
}

inline void LHCb::STTELL1Data::clearEventInfo() 
{
  m_eventInfo.clear();
}

inline unsigned int LHCb::STTELL1Data::dataSize() const 
{
 return m_data.size();  
}

inline unsigned int LHCb::STTELL1Data::TELL1ID() const 
{
  return key();
      
}

inline const std::vector < signed int > & LHCb::STTELL1Data::beetleData(unsigned char beetleID) const 
{
  return m_data[beetleID];
      
}


#endif ///STTELL1Event_STTELL1Data_H
