
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.cpp
 *
 * Implementation file for class : RichHighOccHPDSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHighOccHPDSuppressionTool.cpp,v 1.12 2006-08-31 16:48:48 jonrob Exp $
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
  declareProperty( "PrintHPDSuppressions", m_sumPrint = false );
  declareProperty( "MinHPDFills",       m_minFills   = 20     );
  declareProperty( "AbsoluteMaxHPDOcc", m_overallMax = 100    );
  declareProperty( "OccCutScaleFactor", m_scale      = 4      );
  declareProperty( "MemoryFactor",      m_memory     = 20     );
  declareProperty( "PrintXML",          m_printXML   = false  );
  declareProperty( "ReadOccFromDB",     m_readFromCondDB = true );
  declareProperty( "WhichRich",         m_whichRICH  = "UNDEFINED" );
  declareProperty( "UseRunningOccupancies", m_useRunAv = false );

}

StatusCode RichHighOccHPDSuppressionTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Make sure RICH type is correctly setup
  if ( m_whichRICH != "RICH1" && m_whichRICH != "RICH2" )
  {
    return Error( "Badly formed RICH type : "+m_whichRICH );
  }
  m_rich = ( m_whichRICH == "RICH1" ? Rich::Rich1 : Rich::Rich2 );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // initialise data map
  sc = initOccMap();

  // summary printout of options
  info() << rich() << " pixel suppression options :-" << endreq
         << "  Occupancy memory                      = " << m_memory << endreq
         << "  Occupancy scale factor                = " << m_scale << endreq
         << "  Absolute max HPD occupancy            = " << m_overallMax << endreq;
  if ( m_useRunAv )
  {
    info() << "  Will use running average HPD occupancies" << endreq;
  }
  else
  {
    info() << "  Will use fixed average HPD occupancies" << endreq;
  }

  // return
  return sc;
}

StatusCode RichHighOccHPDSuppressionTool::initOccMap()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Initialise map for all valid HPDs with null entries
  const RichSmartID::Vector & smartIDs = m_richSys->activeHPDRichSmartIDs();
  for ( RichSmartID::Vector::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    m_occMap[ *iHPD ] = HPDData( 0, 0 );
  }

  if ( m_readFromCondDB )
  {
    // Register RICH1
    if ( rich() == Rich::Rich1 )
    {
      updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich1],
                                      &RichHighOccHPDSuppressionTool::umsUpdateRICH1 );
    }
    else
    {
      // Register RICH2
      updMgrSvc()->registerCondition( this, m_condBDLocs[Rich::Rich2],
                                      &RichHighOccHPDSuppressionTool::umsUpdateRICH2 );
    }
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
      verbose() << "Updating HPD " << HPD << " occupancy to " << occ << endreq;
    }
    m_occMap[HPD] = HPDData( m_minFills+1 , occ );
  }

  return StatusCode::SUCCESS;
}

bool
RichHighOccHPDSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{

  // Get occupancy HPD data
  HPDData & data = hpdData(hpdID);

  // Occupancy for this HPD in current event
  const unsigned int occ = smartIDs.size();

  // default is below threshold
  bool suppress       = false;

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
      data.avOcc() =
        ( (m_memory*data.avOcc()) + occ ) / ( m_memory+1 ) ;
    }
    incrementCount = true;
  }

  // is this HPD suppressed
  suppress = ( data.fillCount() >= m_minFills &&
               (occ > m_overallMax || occ > data.avOcc()*m_scale) );
  if ( suppress )
  {
    // Print message
    if ( m_sumPrint )
    {
      std::ostringstream hpd;
      hpd << hpdID;
      Warning( "Fully suppressed     HPD "+hpd.str(), StatusCode::SUCCESS, 10 );
    }
    // clear vector (i.e. actually do the suppression)
    smartIDs.clear();
  }

  // increment count for this HPD
  if ( incrementCount ) { ++(data.fillCount()); }

  // return status
  return suppress;
}

void
RichHighOccHPDSuppressionTool::findHpdData( const LHCb::RichSmartID hpdID ) const
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
  m_lastHPD = hpdID;
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
    const HPDData & d = (*iS).second;
    const RichDAQ::HPDHardwareID hID = m_richSys->hardwareID( (*iS).first );
    const double occ =
      ( d.fillCount() < m_minFills ?
        ( d.fillCount()>0 ? d.avOcc()/d.fillCount() : 0 ) : d.avOcc() );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << (*iS).first << " hID=" << hID << " nMeas=" << d.fillCount()
              << " final occ = " << occ << endreq;
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
  always() << newCond1.toXml() << endreq;

  // RICH2 condition
  Condition newCond2;
  newCond2.addParam( "HPDAvOccupancies",
                     entries[Rich::Rich2], "Average occupancy of RICH2 HPDs" );
  always() << newCond2.toXml() << endreq;

}
