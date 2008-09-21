// $Id: CreateDecReportsHack.cpp,v 1.1 2008-09-21 16:44:57 graven Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "CreateDecReportsHack.h"
#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateDecReportsHack
//
// 2008-09-02 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CreateDecReportsHack );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateDecReportsHack::CreateDecReportsHack( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
CreateDecReportsHack::~CreateDecReportsHack() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateDecReportsHack::execute() {
  LHCb::HltDecReports* decisions = getOrCreate<LHCb::HltDecReports,LHCb::HltDecReports>( m_location );
  setFilterPassed(true);
  return StatusCode::SUCCESS;
};

