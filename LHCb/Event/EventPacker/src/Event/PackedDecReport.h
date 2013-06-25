// $Id: $
#ifndef EVENT_PACKEDDECREPORT_H 
#define EVENT_PACKEDDECREPORT_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {
  
  /** @class PackedDecReport PackedDecReport.h Event/PackedDecReport.h
   *  
   *
   *  @author Olivier Callot
   *  @date   2012-01-20
   */
  static const CLID CLID_PackedDecReport = 1559;

  // Namespace for locations in TDS
  namespace PackedDecReportLocation {
    static const std::string& Default = "pStrip/Phys/DecReports";
  }

  class PackedDecReport : public DataObject {
  public: 
    /// Standard constructor
    PackedDecReport( ) {
      m_data.reserve(1000); 
    }
    
    virtual ~PackedDecReport( ) {}; ///< Destructor
    virtual const CLID& clID()        const { return PackedDecReport::classID(); }
    static  const CLID& classID()           { return CLID_PackedDecReport;       }
    void addReport( unsigned int kk )                { m_data.push_back(kk); }
    void setConfiguredTCK( unsigned int value )      { m_configuredTCK = value; }
    std::vector<unsigned int>& reports()             { return m_data; }
    const std::vector<unsigned int>& reports() const { return m_data; }
    unsigned int configuredTCK() const               { return m_configuredTCK; }

  protected:
    
  private:
    std::vector<unsigned int> m_data;
    unsigned int m_configuredTCK;
    
  };
  
}
#endif // EVENT_PACKEDDECREPORT_H
