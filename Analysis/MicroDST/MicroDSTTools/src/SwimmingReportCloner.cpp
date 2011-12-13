// $Id: SwimmingReportCloner.cpp,v 1.1 2010-08-11 12:48:36 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/SwimmingReport.h"

// local
#include "SwimmingReportCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SwimmingReportCloner
//
// 2011-10-09 : Roel Aaij
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SwimmingReportCloner::SwimmingReportCloner( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : base_class ( type, name , parent ) { }

//=============================================================================

LHCb::SwimmingReport* SwimmingReportCloner::operator() (const LHCb::SwimmingReport* report)
{
  return this->clone(report);
}

//=============================================================================

LHCb::SwimmingReport* SwimmingReportCloner::clone(const LHCb::SwimmingReport* report)
{
  return cloneKeyedContainerItem<Cloner>(report);
}

//=============================================================================
// Destructor
//=============================================================================
SwimmingReportCloner::~SwimmingReportCloner() { }

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SwimmingReportCloner )
