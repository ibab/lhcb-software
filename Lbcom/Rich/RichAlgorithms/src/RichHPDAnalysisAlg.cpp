
//-----------------------------------------------------------------------------
/** @file RichHPDAnalysisAlg.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::HPDAnalysisAlg
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichHPDAnalysisAlg.h"

//-----------------------------------------------------------------------------

using namespace Rich;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HPDAnalysisAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HPDAnalysisAlg::HPDAnalysisAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Rich::AlgBase      ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_taeEvents        ( 1, "" )
{
  declareProperty( "RawEventLocations", m_taeEvents );
  declareProperty( "HPDAnalysisTools",  m_toolNames );
}

//=============================================================================
// Destructor
//=============================================================================
HPDAnalysisAlg::~HPDAnalysisAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HPDAnalysisAlg::initialize()
{
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get decoder tool
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // HPD tools
  if ( m_toolNames.empty() ) return Warning( "No HPD analysis tools configured" );
  info() << "Loading HPD Analysis tools : " << m_toolNames << endmsg;
  for ( std::vector<std::string>::const_iterator iT = m_toolNames.begin();
        iT != m_toolNames.end(); ++iT )
  {
    const Rich::IGenericHPDAnalysisTool * tool(NULL);
    acquireTool( *iT, tool, this );
    m_tools.push_back(tool);
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HPDAnalysisAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  for ( HPDToolNames::const_iterator iLoc = m_taeEvents.begin();
        iLoc != m_taeEvents.end(); ++iLoc )
  {
    sc = sc && processTAEEvent( *iLoc );
  }

  return sc;
}

StatusCode HPDAnalysisAlg::processTAEEvent( const std::string & taeEvent )
{
  // get the decoded data for this tae event
  const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(taeEvent);

  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Rich::DAQ::IngressMap & ingressMap  = iL1Map->second;
    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
      const Rich::DAQ::HPDMap & hpdMap           = ingressInfo.hpdData();
      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {
        const Rich::DAQ::HPDInfo & hpdInfo    = iHPDMap->second;
        const LHCb::RichSmartID  & hpdID      = hpdInfo.hpdID();

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = hpdInfo.smartIDs();

        // Loop over HPD tools
        for ( HPDTools::const_iterator iT = m_tools.begin();
              iT != m_tools.end(); ++iT )
        {
          // Analysis the HPD data
          IGenericHPDAnalysisTool::Results results;
          const StatusCode sc = (*iT)->analyse( hpdID, rawIDs, results );
          if ( sc.isFailure() ) return Warning( "HPD Analysis failed", sc );
          // count any results
          for ( IGenericHPDAnalysisTool::Results::const_iterator iR = results.begin();
                iR != results.end(); ++iR )
          {
            if ( (*iR).status.isSuccess() )
            {
              ++counter( (*iR).message );
            }
            else
            {
              Warning( (*iR).message, StatusCode::SUCCESS, 5 ).ignore();
            }
          }
        }

      }
    }
  }

  return StatusCode::SUCCESS;
}
