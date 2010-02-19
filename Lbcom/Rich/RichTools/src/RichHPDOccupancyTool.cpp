

//-----------------------------------------------------------------------------
/** @file RichHPDOccupancyTool.cpp
 *
 * Implementation file for class : Rich::HPDOccupancyTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 21/03/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichHPDOccupancyTool.h"

// RICH DAQ
using namespace Rich;

DECLARE_TOOL_FACTORY( HPDOccupancyTool );

// Standard constructor
HPDOccupancyTool::HPDOccupancyTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : ToolBase            ( type, name, parent ),
    m_richSys           ( NULL               ),
    m_SmartIDDecoder    ( NULL               ),
    m_condBDLocs        ( Rich::NRiches      ),
    m_updateRunningOccs ( false              )
{

  // Define interface
  declareInterface<IHPDOccupancyTool>(this);

  // job ops
  m_condBDLocs[Rich::Rich1] = "/dd/Conditions/Environment/Rich1/AverageHPDOccupancies";
  m_condBDLocs[Rich::Rich2] = "/dd/Conditions/Environment/Rich2/AverageHPDOccupancies";
  declareProperty( "HPDOccLocs",        m_condBDLocs );
  declareProperty( "MinHPDFills",       m_minFills     = 20     );
  declareProperty( "MemoryFactor",      m_memory       = 20     );
  declareProperty( "PrintXML",          m_printXML     = false  );
  declareProperty( "ReadOccFromDB",     m_readFromCondDB = true );
  declareProperty( "WhichRich",         m_whichRICH    = "RICH1andRICH2" );
  declareProperty( "UseRunningOccupancies", m_useRunAv = false  );

}

StatusCode HPDOccupancyTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // check config is OK
  if ( m_whichRICH != "RICH1andRICH2" &&
       m_whichRICH != "RICH1"         &&
       m_whichRICH != "RICH2"           )
  {
    return Error( "Badly formed RICH type : "+m_whichRICH );
  }

  // Setup incident services if needed
  if ( m_useRunAv )
  {
    incSvc()->addListener( this, IncidentType::BeginEvent );
    m_updateRunningOccs = true;
  }

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // get tools
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // initialise data map
  sc = initOccMap();

  // summary printout of options
  debug() << m_whichRICH << " pixel suppression options :-" << endmsg
          << "  Occupancy memory                      = " << m_memory << endmsg;
  if ( m_useRunAv )
  {
    debug() << "  Will use running average HPD occupancies" << endmsg;
  }
  else
  {
    debug() << "  Will use fixed average HPD occupancies" << endmsg;
  }

  // return
  return sc;
}

void HPDOccupancyTool::handle ( const Incident& )
{
  // Only one incident type registered, so do not bother to check type
  m_updateRunningOccs = true;
}

StatusCode HPDOccupancyTool::initOccMap()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Initialise map for all valid HPDs with null entries
  const LHCb::RichSmartID::Vector & smartIDs = m_richSys->activeHPDRichSmartIDs();
  for ( LHCb::RichSmartID::Vector::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    m_occMap[ *iHPD ] = HPDData( 0, 0 );
  }

  if ( m_readFromCondDB )
  {
    // Register RICH1
    if ( m_whichRICH == "RICH1" || m_whichRICH == "RICH1andRICH2" )
    {
      updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich1],
                                      &HPDOccupancyTool::umsUpdateRICH1 );
    }
    if ( m_whichRICH == "RICH2" || m_whichRICH == "RICH1andRICH2" )
    {
      // Register RICH2
      updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich2],
                                      &HPDOccupancyTool::umsUpdateRICH2 );
    }
    // force first updates
    sc = updMgrSvc()->update(this);
    if (sc.isFailure()) return Error ( "Failed first UMS update", sc );
  }
  else
  {
    info() << "Using NULL starting HPD occupancies" << endmsg
           << "Min # measurements before suppressing = " << m_minFills << endmsg;
  }

  return sc;
}

StatusCode HPDOccupancyTool::umsUpdateRICH1()
{
  return initOccMap( Rich::Rich1 );
}

StatusCode HPDOccupancyTool::umsUpdateRICH2()
{
  return initOccMap( Rich::Rich2 );
}

StatusCode HPDOccupancyTool::initOccMap( const Rich::DetectorType rich )
{
  info() << "Update triggered for " << rich << " HPD average occupancies" << endmsg;

  // read data from conditions
  const Condition * data = getDet<Condition>(m_condBDLocs[rich]);
  // vector of data values
  const std::vector<std::string> & values = data->paramAsStringVect( "Occupancies" );

  // loop over data values from cond DB
  for ( std::vector<std::string>::const_iterator iS = values.begin();
        iS != values.end(); ++iS )
  {
    // extract numbers from string
    const int slash       = (*iS).find_first_of( "/" );
    if ( slash == 0 ) return Error( "Badly formed data value = " + *iS );
    const LHCb::RichSmartID HPD ( boost::lexical_cast<int>    ( (*iS).substr(0,slash) ) );
    const double            occ ( boost::lexical_cast<double> ( (*iS).substr(slash+1) ) );
    // update local data map
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Updating HPD " << HPD << " occupancy to " << occ << endmsg;
    }
    m_occMap[HPD] = HPDData( m_minFills+1 , occ );
  }

  return StatusCode::SUCCESS;
}

HPDOccupancyTool::HPDData &
HPDOccupancyTool::hpdData( const LHCb::RichSmartID hpdID ) const
{
  // update occupancies
  if ( m_updateRunningOccs )
  {
    // Must set to false before call to updateOccupancies() to avoid infinite loop ...
    m_updateRunningOccs = false; 
    updateOccupancies();
  }
  // if different HPD, search for new data object
  if ( m_lastHPD != hpdID ) { findHpdData(hpdID); }
  // return data object
  return *m_currentData;
}

double
HPDOccupancyTool::averageOccupancy( const LHCb::RichSmartID hpdID ) const
{
  // valid HPD ID
  if ( !hpdID.isValid() ) return 0;
  // Get occupancy HPD data
  const HPDData & data = hpdData(hpdID);
  // return the average occupancy
  return data.avOcc();
}

void
HPDOccupancyTool::updateOccupancies() const
{

  // get the decoded data for this tae event
  const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs();

  // loop over decoded data
  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Rich::DAQ::IngressMap & ingressMap  = iL1Map->second;
    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
      const Rich::DAQ::HPDMap & hpdMap = ingressInfo.hpdData();
      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {
        const Rich::DAQ::HPDInfo & hpdInfo    = iHPDMap->second;
        const LHCb::RichSmartID  & hpdID      = hpdInfo.hpdID();

        // Valid HPD ID
        if ( ! hpdID.isValid() )          { continue; }
        // inhibited HPD ?
        if ( hpdInfo.header().inhibit() ) { continue; }

        // Get occupancy HPD data
        HPDData & data = hpdData(hpdID);

        // Occupancy for this HPD in current event
        const unsigned int occ = hpdInfo.smartIDs().size();

        bool incrementCount = false;
        if      ( data.fillCount() <  m_minFills )
        {
          // Not yet enough sampling data, so just update
          data.avOcc() += occ;
          incrementCount = true;
        }
        else if ( m_useRunAv )
        {
          if ( data.fillCount() == m_minFills )
          {
            // Now enough data so update and normalise
            data.avOcc() += occ;
            data.avOcc() /= (1+m_minFills);
          }
          else
          {
            // update running average occ for this HPD
            data.avOcc() = ( (m_memory*data.avOcc()) + occ ) / ( m_memory+1 ) ;
          }
          incrementCount = true;
        }

        // increment count for this HPD
        if ( incrementCount ) { ++(data.fillCount()); }

      }
    }
  }

}

void
HPDOccupancyTool::findHpdData( const LHCb::RichSmartID hpdID ) const
{
  // get data for this HPD
  OccMap::iterator iD = m_occMap.find(hpdID);
  if ( iD == m_occMap.end() )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << hpdID;
    throw GaudiException( mess.str(),
                          "Rich::HPDOccupancyTool",
                          StatusCode::FAILURE );
  }
  m_currentData = &(*iD).second;
  m_lastHPD = hpdID;
}

StatusCode HPDOccupancyTool::finalize()
{
  // Print XML ?
  if ( m_printXML ) createHPDBackXML();
  // Execute base class method
  return ToolBase::finalize();
}

void HPDOccupancyTool::createHPDBackXML() const
{

  // Vectors for conditions entries
  std::vector<std::string> entries[Rich::NRiches];

  // loop over final occupancies
  for ( OccMap::const_iterator iS = m_occMap.begin(); iS != m_occMap.end(); ++iS )
  {
    const HPDData & d = (*iS).second;
    const Rich::DAQ::HPDHardwareID hID = m_richSys->hardwareID( (*iS).first );
    const double occ =
      ( d.fillCount() < m_minFills ?
        ( d.fillCount()>0 ? d.avOcc()/d.fillCount() : 0 ) : d.avOcc() );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << (*iS).first << " hID=" << hID << " nMeas=" << d.fillCount()
              << " final occ = " << occ << endmsg;
    }
    // Create condition string
    if ( d.fillCount() > m_minFills )
    {
      const std::string entry =
        boost::lexical_cast<std::string>( (int)(*iS).first ) + "/" +
        boost::lexical_cast<std::string>( occ );
      entries[(*iS).first.rich()].push_back( entry );
    }
  }

  // Rich1 Condition
  Condition newCond1;
  newCond1.addParam( "HPDAvOccupancies",
                     entries[Rich::Rich1], "Average occupancy of RICH1 HPDs" );
  always() << newCond1.toXml() << endmsg;

  // RICH2 condition
  Condition newCond2;
  newCond2.addParam( "HPDAvOccupancies",
                     entries[Rich::Rich2], "Average occupancy of RICH2 HPDs" );
  always() << newCond2.toXml() << endmsg;

}
