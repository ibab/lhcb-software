
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.cpp
 *
 * Implementation file for class : RichHighOccHPDSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHighOccHPDSuppressionTool.cpp,v 1.3 2006-03-22 09:51:52 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHighOccHPDSuppressionTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichHighOccHPDSuppressionTool>          s_factory ;
const        IToolFactory& RichHighOccHPDSuppressionToolFactory = s_factory ;

// Standard constructor
RichHighOccHPDSuppressionTool::
RichHighOccHPDSuppressionTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_richSys    ( NULL               ),
    m_condBDLocs ( Rich::NRiches      )
{

  // Define interface
  declareInterface<IRichPixelSuppressionTool>(this);

  // job ops
  m_condBDLocs[Rich::Rich1] = "/dd/Conditions/Environment/Rich1/AverageHPDOccupancies";
  m_condBDLocs[Rich::Rich2] = "/dd/Conditions/Environment/Rich2/AverageHPDOccupancies";
  declareProperty( "HPDOccLocs",        m_condBDLocs );
  declareProperty( "MinHPDFills",       m_minFills   = 20   );
  declareProperty( "AbsoluteMaxHPDOcc", m_overallMax = 100  );
  declareProperty( "OccCutScaleFactor", m_scale      = 4    );
  declareProperty( "MemoryFactor",      m_memory     = 20   );
  declareProperty( "PrintXML",          m_printXML   = true );
  declareProperty( "ReadOccFromDB",     m_readFromCondDB = true );

}

StatusCode RichHighOccHPDSuppressionTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // initialise data map
  sc = initOccMap();

  // summary printout of options
  info() << "Occupancy memory                      = " << m_memory << endreq
         << "Occupancy scale factor                = " << m_scale << endreq
         << "Absolute Max HPD occupancy            = " << m_overallMax << endreq;

  // return
  return sc;
}

StatusCode RichHighOccHPDSuppressionTool::initOccMap( )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Initialise map for all valid HPDs with null entries
  const RichSmartID::Vector & smartIDs = m_richSys->activeHPDRichSmartIDs();
  for ( RichSmartID::Vector::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    m_occMap[ *iHPD ] = Data( 0, 0 );
  }

  if ( m_readFromCondDB )
  {
    // Register RICH1
    updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich1],
                                    &RichHighOccHPDSuppressionTool::umsUpdateRICH1 );
    // Register RICH2
    updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich2],
                                    &RichHighOccHPDSuppressionTool::umsUpdateRICH2 );
    // force first updates
    sc = updMgrSvc()->update(this);
    if (sc.isFailure()) return Error ( "Failed first UMS update", sc );
  }
  else
  {
    info() << "Using NULL starting HPD occupancies" << endreq
           << "Min # measurements before suppressing = " << m_minFills << endreq;
  }

  return sc;
}

StatusCode RichHighOccHPDSuppressionTool::umsUpdateRICH1()
{
  return initOccMap( Rich::Rich1 );
}

StatusCode RichHighOccHPDSuppressionTool::umsUpdateRICH2()
{
  return initOccMap( Rich::Rich2 );
}

StatusCode RichHighOccHPDSuppressionTool::initOccMap( const Rich::DetectorType rich )
{
  info() << "Update triggered for " << rich << " HPD average occupancies" << endreq;

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
    const RichSmartID HPD ( boost::lexical_cast<int>    ( (*iS).substr(0,slash) ) );
    const double      occ ( boost::lexical_cast<double> ( (*iS).substr(slash+1) ) );
    // update local data map
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Updating " << HPD << " occupancy to " << occ << endreq;
    }
    m_occMap[HPD] = Data( m_minFills+1 , occ );
  }

  return StatusCode::SUCCESS;
}

bool
RichHighOccHPDSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{

  // get data for this HPD
  OccMap::iterator iD = m_occMap.find(hpdID);
  if ( iD == m_occMap.end() )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << hpdID;
    throw GaudiException( mess.str(),
                          "RichHighOccHPDSuppressionTool",
                          StatusCode::FAILURE );
  }
  m_currentData = &(*iD).second;

  // Occupancy for this HPD in current event
  const unsigned int occ = smartIDs.size();

  // default is below threshold
  bool aboveThres = false;

  if      ( m_currentData->first <  m_minFills )
  {
    // Not yet enough sampling data, so just update
    m_currentData->second += occ;
  }
  else if ( m_currentData->first == m_minFills )
  {
    // Now enough data so update and normalise
    m_currentData->second += occ;
    m_currentData->second /= (1+m_minFills);
  }
  else
  {
    // update running average occ for this HPD
    m_currentData->second = 
      ( (m_memory*m_currentData->second) + occ ) / ( m_memory+1 ) ;
  }

  // is this HPD suppressed
  aboveThres = ( m_currentData->first >= m_minFills &&
                 (occ > m_overallMax || occ > m_currentData->second*m_scale) );

  // is this HPD suppressed ?
  if ( aboveThres )
  {
    // Print message
    std::ostringstream hpd;
    hpd << hpdID;
    Warning( "Fully suppressed     HPD "+hpd.str(), StatusCode::SUCCESS, 3 );
    // clear vector
    smartIDs.clear();
  }

  // increment count for this HPD
  ++m_currentData->first;

  // return status
  return aboveThres;
}

StatusCode RichHighOccHPDSuppressionTool::finalize()
{
  // Print XML ?
  if ( m_printXML ) createHPDBackXML();
  // Execute base class method
  return RichToolBase::finalize();
}

void RichHighOccHPDSuppressionTool::createHPDBackXML() const
{

  // Vectors for conditions entries
  std::vector<std::string> entries[Rich::NRiches];

  // loop over final occupancies
  for ( OccMap::const_iterator iS = m_occMap.begin(); iS != m_occMap.end(); ++iS )
  {
    const Data & d = (*iS).second;
    const RichDAQ::HPDHardwareID hID = m_richSys->hardwareID( (*iS).first );
    const double occ =
      ( d.first < m_minFills ? ( d.first>0 ? d.second/d.first : 0 ) : d.second );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << (*iS).first << " hID=" << hID << " nMeas=" << d.first
              << " final occ = " << occ << endreq;
    }
    // Create condition string
    if ( d.first > m_minFills )
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
  info() << newCond1.toXml() << endreq;

  // RICH2 condition
  Condition newCond2;
  newCond2.addParam( "HPDAvOccupancies",
                     entries[Rich::Rich2], "Average occupancy of RICH2 HPDs" );
  info() << newCond2.toXml() << endreq;

}
