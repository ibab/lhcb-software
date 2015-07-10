
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
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, NULL, true );

  // HPD tools
  if ( m_toolNames.empty() ) return Warning( "No HPD analysis tools configured" );
  info() << "Loading HPD Analysis tools : " << m_toolNames << endmsg;
  for ( const auto& t : m_toolNames )
  {
    const Rich::IGenericHPDAnalysisTool * tool(NULL);
    acquireTool( t, tool, this );
    m_tools.push_back(tool);
  }

  // return
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HPDAnalysisAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  for ( const auto& evt : m_taeEvents ) { sc = sc && processTAEEvent(evt); }

  return sc;
}

StatusCode HPDAnalysisAlg::processTAEEvent( const std::string & taeEvent )
{
  // get the decoded data for this tae event
  const auto & l1Map = m_SmartIDDecoder->allRichSmartIDs(taeEvent);

  for ( const auto& L1 : l1Map )
  {
    for ( const auto& ingress : L1.second )
    {
      for ( const auto& hpd : ingress.second.hpdData() )
      {
        // HPD ID
        const LHCb::RichSmartID & hpdID = hpd.second.hpdID();

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = hpd.second.smartIDs();

        // Loop over HPD tools
        for ( const auto* t : m_tools )
        {
          // Analysis the HPD data
          IGenericHPDAnalysisTool::Results results;
          const StatusCode sc = t->analyse( hpdID, rawIDs, results );
          if ( sc.isFailure() )
          {
            Warning( "HPD Analysis failed", sc ).ignore(); 
          }
          else
          {
            // count any results
            for ( const auto& res : results )
            {
              if ( res.status.isSuccess() ) { ++counter( res.message ); }
              else
              { Warning( res.message, StatusCode::SUCCESS, 5 ).ignore(); }
            }
          }
        } // tools loop

      }
    }
  }

  return StatusCode::SUCCESS;
}
