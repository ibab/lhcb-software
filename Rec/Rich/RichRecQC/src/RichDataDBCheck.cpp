
//-----------------------------------------------------------------------------
/** @file RichDataDBCheck.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::DataDBCheck
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichDataDBCheck.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DataDBCheck )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataDBCheck::DataDBCheck( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Rich::HistoAlgBase ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_RichSys          ( NULL  ),
    m_taeEvents        ( 1, "" )
{
  declareProperty( "RawEventLocations", m_taeEvents );
  declareProperty( "NumErrorMess",      m_nErrorMess = 10 );
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
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, NULL, true );

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
  const L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(m_taeEvents);

  L0IDInfoCount l0Count;

  // Loop over data
  for ( L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Level1HardwareID & l1HardID = iL1Map->first;
    const Level1LogicalID   l1LogID   = m_RichSys->level1LogicalID(l1HardID);
    const IngressMap & ingressMap     = iL1Map->second;
    for ( IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const L1IngressID & l1IngressID = iIngressMap->first;
      const IngressInfo & ingressInfo = iIngressMap->second;
      const HPDMap & hpdMap = ingressInfo.hpdData();
      for ( HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {
        const Level1Input & l1Input       = iHPDMap->first;
        const HPDInfo & hpdInfo           = iHPDMap->second;
        const LHCb::RichSmartID  & hpdID  = hpdInfo.hpdID();
        const HPDInfo::Header & hpdHeader = hpdInfo.header();
        const Level0ID l0ID               = hpdHeader.l0ID();  
        const Rich::DetectorType  rich    = hpdID.rich();

        // Only do the DB checks on valid data
        if ( hpdHeader.inhibit() || !hpdID.isValid() ) continue;

        // use a try block in case of DB lookup errors
        try
        {

          // look up information from DB for this HPD
          const HPDHardwareID    db_hpdHardID = m_RichSys->hardwareID(hpdID);
          const Level1HardwareID db_l1HardID  = m_RichSys->level1HardwareID(hpdID);
          const Level1Input      db_l1Input   = m_RichSys->level1InputNum(hpdID);
          const Level0ID         db_l0ID      = m_RichSys->level0ID(hpdID);

          // compare to that in the data itself
          compare( "Level1HardwareID-Data-DB", hpdID, l0ID, l1HardID, db_l1HardID );
          compare( "Level1Input-Data-DB",      hpdID, l0ID, l1Input,  db_l1Input  );
          compare( "Level0ID-Data-DB",         hpdID, l0ID, l0ID,     db_l0ID     );

          // Internal consistency checks
          // Get l1HardID from RICH and l1LogicalID
          const Level1HardwareID new_l1HardID = m_RichSys->level1HardwareID(rich,l1LogID);
          compare( "Level1HardwareID-DB-DB", hpdID, l0ID, new_l1HardID, l1HardID );
          // HPD hardware ID
          const HPDHardwareID new_hpdHardID = m_RichSys->pdHardwareID(l1HardID,l1Input);
          compare( "HPDHardwareID-DB-DB", hpdID, l0ID, new_hpdHardID, db_hpdHardID );
          const L1InputWithinIngress l1InputWithinIngress = l1Input.l1InputWithinIngress();
          // L1 input
          const Level1Input new_l1Input(l1IngressID,l1InputWithinIngress);
          compare( "L1Input-DB-DB", hpdID, l0ID, l1Input, new_l1Input );

          // Is this L0ID already in the map... If so this is an error.
          L0IDInfoCount::const_iterator iID = l0Count.find(l0ID);
          if ( iID != l0Count.end() )
          {
            // Construct and send the warning
            std::ostringstream mess;
            mess << "HPD L0ID " << l0ID << " appears twice in the data. ["
                 << " L1HardwareID=" << iID->second.l1HardID
                 << " Input=" <<  iID->second.l1Input
                 << " ] and [ L1HardwareID=" <<  l1HardID << " Input=" << l1Input << " ]";
            Warning( mess.str(), StatusCode::FAILURE, m_nErrorMess ).ignore();
          }
          else
          {
            // Add to L0ID data map
            l0Count[l0ID] = L0IDInfo(l1HardID,l1Input);
          }

        }
        catch ( const GaudiException & excpt )
        {
          Warning( excpt.message() + " " + excpt.tag() ).ignore();
        }

      } // loop over HPDs

    } // loop over ingresses

  } // loop over L1 boards

  return StatusCode::SUCCESS;
}

//=============================================================================
