// $Id: HltTriggerRawConversion.cpp,v 1.1 2009-03-25 10:29:09 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/AlgFactory.h" 

class DataObject;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTriggerRawConversion
//
//-----------------------------------------------------------------------------
class HltTriggerRawConversion : public Algorithm {
public:
  HltTriggerRawConversion( const std::string& , ISvcLocator* );
  ~HltTriggerRawConversion() {}; 
  StatusCode execute();
private:
  std::vector<std::string> m_paths;
  bool m_passFailed;
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTriggerRawConversion );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTriggerRawConversion::HltTriggerRawConversion( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
    declareProperty("Paths", m_paths );
    declareProperty("PassFailed", m_passFailed = false );
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTriggerRawConversion::execute() {
    IDataProviderSvc* dps = eventSvc();
    bool pass = false;
    for (std::vector<std::string>::const_iterator i = m_paths.begin();i!=m_paths.end();++i) {
        DataObject *p(0);
        StatusCode sc = dps->findObject( *i, p );
        if (sc.isFailure()) { pass = true; }
    }
    if (m_passFailed) setFilterPassed(pass);
    return StatusCode::SUCCESS;
};
