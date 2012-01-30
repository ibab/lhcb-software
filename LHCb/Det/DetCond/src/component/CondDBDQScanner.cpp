// Include files

// From Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCb
#include "DetCond/ICondDBAccessSvc.h"

// local
#include "CondDBDQScanner.h"

// ----------------------------------------------------------------------------
// Implementation file for class: CondDBDQScanner
//
// 04/11/2011: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(CondDBDQScanner)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
CondDBDQScanner::CondDBDQScanner(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent)
{

  declareProperty("ConditionPath",
                  m_condPath = "/Conditions/DQ/Flags",
                  "Path in the Conditions Database where to find the Data "
                  "Quality condition.");

  declareProperty("CondDBAccessSvc",
                  m_condDBAccessSvcName = "CondDBAccessSvc/DQFLAGS",
                  "Service implementing the ICondDBReader interface to be used "
                  "to access the CondDB.");

}

CondDBDQScanner::~CondDBDQScanner() {}

IDQFilter::FlagsType CondDBDQScanner::scan(const Gaudi::Time & since, const Gaudi::Time & until) const
{
  IDQFilter::FlagsType flags;


  return flags;
}




StatusCode CondDBDQScanner::initialize()
{
  StatusCode sc = base_class::initialize();
  if (sc.isFailure()) return sc;

  m_condDB = service(m_condDBAccessSvcName);
  if (UNLIKELY(!m_condDB.isValid())) {
    error() << "Cannot get the CondDBAccessSvc implementation " << m_condDBAccessSvcName << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}



StatusCode CondDBDQScanner::finalize()
{
  m_condDB.reset(); // release the ICondDBReader service

  return base_class::finalize();
}



// ============================================================================
