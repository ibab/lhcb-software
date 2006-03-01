
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.cpp
 *
 * Implementation file for class : RichHighOccHPDSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHighOccHPDSuppressionTool.cpp,v 1.1 2006-03-01 09:56:12 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichHighOccHPDSuppressionTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichHighOccHPDSuppressionTool>          s_factory ;
const        IToolFactory& RichHighOccHPDSuppressionToolFactory = s_factory ;

// Standard constructor
RichHighOccHPDSuppressionTool::RichHighOccHPDSuppressionTool( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_richSys    ( 0                  )
{

  // Define interface
  declareInterface<IRichPixelSuppressionTool>(this);

  // job ops
  declareProperty( "MinHPDFills",       m_minFills   = 20  );
  declareProperty( "AbsoluteMaxHPDOcc", m_overallMax = 100 );
  declareProperty( "OccCutScaleFactor", m_scale      = 4   );
  declareProperty( "MemoryFactor",      m_memory     = 20  );

}

StatusCode RichHighOccHPDSuppressionTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // Initialise map for all valid HPDs
  const RichSmartID::Vector & smartIDs = m_richSys->activeHPDRichSmartIDs();
  for ( RichSmartID::Vector::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    m_occMap[ *iHPD ] = Data( 0, 0 );
  }

  // summary printout of options
  info() << "Occupancy memory                    = " << m_memory << endreq
         << "Occupancy scale factor              = " << m_scale << endreq
         << "Absoulate Max HPD occupancy         = " << m_overallMax << endreq
         << "Min measurements before suppressing = " << m_minFills << endreq;

  return sc;
}

StatusCode RichHighOccHPDSuppressionTool::finalize()
{
  // Print Final occupancies
  if ( msgLevel(MSG::DEBUG) )
  {
    // Create a sorted map
    typedef Rich::Map< const LHCb::RichSmartID, Data > SortMap;
    SortMap sortMap;
    for ( OccMap::const_iterator iM = m_occMap.begin(); iM != m_occMap.end(); ++iM )
    {
      sortMap[ (*iM).first ] = (*iM).second;
    }
    for ( SortMap::const_iterator iS = sortMap.begin(); iS != sortMap.end(); ++iS )
    {
      const Data & d = (*iS).second;
      const RichDAQ::HPDHardwareID hID = m_richSys->hardwareID( (*iS).first );
      const double occ =
        ( d.first < m_minFills ? ( d.first>0 ? d.second/d.first : 0 ) : d.second );
      debug() << (*iS).first << " hID=" << hID << " nMeas=" << d.first
              << " final occ = " << occ << endreq;
    }

  }

  // Execute base class method
  return RichToolBase::finalize();
}

bool
RichHighOccHPDSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{

  // get data for this HPD
  Data & data = m_occMap[hpdID];

  const unsigned int occ = smartIDs.size();

  // default is below threshold
  bool aboveThres = false;

  if      ( data.first < m_minFills )
  {
    data.second += occ;
  }
  else if ( data.first == m_minFills )
  {
    data.second /= m_minFills;
  }
  else
  {
    // update running average occ for this HPD
    data.second = ( (m_memory*data.second) + occ ) / ( m_memory+1 ) ;

    // is this HPD suppressed
    aboveThres = ( occ > m_overallMax || occ > data.second*m_scale );

    if ( aboveThres )
    {
      std::ostringstream hpd;
      hpd << hpdID;
      Warning( "Suppressed HPD "+hpd.str(), StatusCode::SUCCESS, 10 );
    }

  }

  // increment count for this HPD
  ++data.first;

  // if suppressed, clear vector
  if ( aboveThres ) smartIDs.clear();

  // return status
  return aboveThres;
}
