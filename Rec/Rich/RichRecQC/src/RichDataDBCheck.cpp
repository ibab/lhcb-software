
//-----------------------------------------------------------------------------
/** @file RichDataDBCheck.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::DataDBCheck
 *
 *  CVS Log :-
 *  $Id: RichDataDBCheck.cpp,v 1.3 2008-10-20 15:08:43 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichDataDBCheck.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DataDBCheck );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataDBCheck::DataDBCheck( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Rich::HistoAlgBase ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL ),
    m_RichSys          ( NULL ),
    m_taeEvents        (   1, ""            )
{
  declareProperty( "RawEventLocations", m_taeEvents );
}

//=============================================================================
// Destructor
//=============================================================================
DataDBCheck::~DataDBCheck() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DataDBCheck::initialize()
{
  const StatusCode sc = Rich::HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DataDBCheck::execute()
{

  // get the raw data
  const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(m_taeEvents);

  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Rich::DAQ::Level1HardwareID l1HardID = iL1Map->first;
    const Rich::DAQ::IngressMap & ingressMap  = iL1Map->second;
    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const Rich::DAQ::L1IngressID   l1IngressID = iIngressMap->first;
      const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
      const Rich::DAQ::HPDMap & hpdMap = ingressInfo.hpdData();
      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {
        const Rich::DAQ::Level1Input l1Input = iHPDMap->first;
        const Rich::DAQ::HPDInfo & hpdInfo   = iHPDMap->second;
        const LHCb::RichSmartID  & hpdID     = hpdInfo.hpdID();

        // use a try block in case of DB lookup errors
        try
        {
          // look up information from DB for this HPD
          const Rich::DAQ::Level1HardwareID db_l1HardID = m_RichSys->level1HardwareID(hpdID);
          const Rich::DAQ::Level1Input      db_l1Input  = m_RichSys->level1InputNum(hpdID);
          const Rich::DAQ::Level0ID         db_l0ID     = m_RichSys->level0ID(hpdID);
          // compare to that in the data itself
          compare( "Level1HardwareID", l1HardID,                db_l1HardID );
          compare( "Level1Input",      l1Input,                 db_l1Input  );
          compare( "Level0ID",         hpdInfo.header().l0ID(), db_l0ID     );
        }
        catch ( const GaudiException & excpt )
        {
          Error( excpt.message() ).ignore();
        }

      } // loop over HPDs

    } // loop over ingresses

  } // loop over L1 boards

  return StatusCode::SUCCESS;
}

//=============================================================================
