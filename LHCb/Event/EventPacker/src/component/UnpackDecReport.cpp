// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/LinkManager.h"

#include "Event/StandardPacker.h"
#include "Event/HltDecReports.h"
#include "Event/PackedDecReport.h"

// local
#include "UnpackDecReport.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackDecReport
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackDecReport )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackDecReport::UnpackDecReport( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedDecReportLocation::Default );
  declareProperty( "OutputName", m_outputName = "Strip/Phys/DecReports" );
  declareProperty( "AlwaysCreateOutput",      m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackDecReport::~UnpackDecReport() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackDecReport::execute() 
{

  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedDecReport>(m_inputName) ) return StatusCode::SUCCESS;
  const auto * dst = 
    getOrCreate<LHCb::PackedDecReport,LHCb::PackedDecReport>( m_inputName );
  LHCb::HltDecReports* newReport = new LHCb::HltDecReports();
  put( newReport, m_outputName );
  
  newReport->setConfiguredTCK( dst->configuredTCK() );
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Unpacked TCK = " << newReport->configuredTCK() << endmsg;     
  
  newReport->setTaskID( 0 );
  for ( const auto & R : dst->reports() )
  {
    LHCb::HltDecReport tmp(R);
    LinkManager::Link* myLink = dst->linkMgr()->link( tmp.intDecisionID()-1 );  // Was stored with +1.
    if ( nullptr == myLink )
    {
      info() << "No link table entry for " << tmp.intDecisionID() << endmsg;
    }
    else
    {
      const std::string & name = myLink->path();
      tmp.setIntDecisionID( 1 );
      newReport->insert( name, tmp );
      if ( msgLevel(MSG::DEBUG) ) 
        debug() << format( "restored report %8.8x link ID %3d", tmp.decReport(), myLink->ID() ) 
                << " name " << name << endmsg;     
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
