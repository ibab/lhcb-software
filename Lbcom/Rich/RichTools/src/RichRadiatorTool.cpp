
//-----------------------------------------------------------------------------
/** @file RichRadiatorTool.cpp
 *
 *  Implementation file for tool : Rich::RadiatorTool
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
//-----------------------------------------------------------------------------

// Include files

// local
#include "RichRadiatorTool.h"

// Rich Det
#include "RichDet/DeRich.h"

//=============================================================================

Rich::RadiatorTool::RadiatorTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::ToolBase ( type, name, parent   ),
    m_radiators    ( Rich::NRadiatorTypes )
{
  // interface
  declareInterface<IRadiatorTool>(this);
  // JOs
  declareProperty( "ExcludedFullTileIDs", m_excludedAeroTiles );
  // Debug messages
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================

Rich::RadiatorTool::~RadiatorTool() {}

//=============================================================================

StatusCode Rich::RadiatorTool::finalize()
{
  updMgrSvc()->unregister(this);
  return Rich::ToolBase::finalize();
}

//=============================================================================

void Rich::RadiatorTool::loadRadiator( const Rich::RadiatorType radiator )
{
  StatusCode sc = StatusCode::SUCCESS;
  if      ( radiator == Rich::Rich1Gas )
  {
    sc = updateRich1Gas();
    updMgrSvc()->registerCondition( this, DeRichLocations::Rich1Gas,
                                    &Rich::RadiatorTool::updateRich1Gas );
  }
  else if ( radiator == Rich::Rich2Gas )
  {
    sc = updateRich2Gas();
    updMgrSvc()->registerCondition( this, DeRichLocations::Rich2Gas,
                                    &Rich::RadiatorTool::updateRich2Gas );
  }
  else if ( radiator == Rich::Aerogel )
  {
    sc = updateAerogel();
    updMgrSvc()->registerCondition( this, DeRichLocations::Aerogel,
                                    &Rich::RadiatorTool::updateAerogel );
  }
  if ( sc.isFailure() )
  {
    std::ostringstream mess;
    mess << "Problem loading radiator " << radiator;
    Exception( mess.str() );
  }
}

//=============================================================================
// UMS update
//=============================================================================

StatusCode Rich::RadiatorTool::updateRich1Gas()
{
  m_radiators[Rich::Rich1Gas].clear();
  m_radiators[Rich::Rich1Gas].push_back( getDet<DeRichRadiator>(DeRichLocations::Rich1Gas) );
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode Rich::RadiatorTool::updateRich2Gas()
{
  m_radiators[Rich::Rich2Gas].clear();
  m_radiators[Rich::Rich2Gas].push_back( getDet<DeRichRadiator>(DeRichLocations::Rich2Gas) );
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode Rich::RadiatorTool::updateAerogel()
{
  m_radiators[Rich::Aerogel].clear();
  const DeRichMultiSolidRadiator * aerogel
    = getDet<DeRichMultiSolidRadiator>( DeRichLocations::Aerogel );
  m_radiators[Rich::Aerogel].reserve( aerogel->radiators().size() );
  for ( DeRichRadiator::Vector::const_iterator dRad = aerogel->radiators().begin();
        dRad != aerogel->radiators().end(); ++dRad )
  {
    const DeRichAerogelRadiator* d = dynamic_cast<const DeRichAerogelRadiator*>(*dRad);
    if (!d) return Error( "Failed to cast to DeRichAerogelRadiator" );
    if ( std::find( m_excludedAeroTiles.begin(),
                    m_excludedAeroTiles.end(),
                    d->primaryTileID() ) == m_excludedAeroTiles.end() )
    {
      m_radiators[Rich::Aerogel].push_back( *dRad );
    }
    else
    {
      std::ostringstream mess;
      mess << "Ignoring Aerogel tile " << d->primaryTileID();
      Warning( mess.str(), StatusCode::SUCCESS ).ignore();
    }
  }

  // Sort by distance from beam line
  std::stable_sort( m_radiators[Rich::Aerogel].begin(),
                    m_radiators[Rich::Aerogel].end(),
                    SortByDistFromBeam() );

  return StatusCode::SUCCESS;
}

//=============================================================================

unsigned int
Rich::RadiatorTool::intersections( const Gaudi::XYZPoint& globalPoint,
                                   const Gaudi::XYZVector& globalVector,
                                   const Rich::RadiatorType radiator,
                                   Rich::RadIntersection::Vector & intersections ) const
{

  // tally for the number of intersections
  unsigned int totalIntersections( 0 );

  // Make sure radiator is loaded
  if ( m_radiators[radiator].empty() )
  {
    const_cast<Rich::RadiatorTool*>(this)->loadRadiator(radiator);
  }

  // loop over all volumes for given radiator
  /** @todo With aerogel sub-tiles, there are now a lot of volumes
   *        need to investigate a faster way to do this search than
   *        just looping over them all */
  for ( RichRadiators::const_iterator iR = m_radiators[radiator].begin();
        iR != m_radiators[radiator].end(); ++iR )
  {
    // entry and exit points
    Gaudi::XYZPoint entry, exit;
    // do we intersect ?
    if ( (*iR)->intersectionPoints( globalPoint,
                                    globalVector,
                                    entry,
                                    exit ) )
    {
      // save this intersection
      intersections.push_back( Rich::RadIntersection( entry, globalVector,
                                                      exit,  globalVector,
                                                      *iR ) );
      ++totalIntersections;
    }
  }

  // If more than one intersection sort into increasing z position
  if ( totalIntersections > 1 )
  {
    Rich::RadIntersection::Sorter::sortByZ( intersections );
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << radiator << " Intersections :";
    for ( Rich::RadIntersection::Vector::const_iterator iS = intersections.begin();
          iS != intersections.end(); ++iS )
    {
      debug() << " " << (*iS).entryPoint() << "->" << (*iS).exitPoint();
    }
    debug() << endmsg;
  }

  // return the number of intersections
  return totalIntersections;
}

//=============================================================================

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RadiatorTool )

//=============================================================================
