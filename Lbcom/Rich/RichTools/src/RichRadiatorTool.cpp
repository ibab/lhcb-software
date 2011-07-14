
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

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRadiatorTool.h"

// Rich Det
#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RadiatorTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RadiatorTool::RadiatorTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::ToolBase ( type, name, parent   ),
    m_radiators    ( Rich::NRadiatorTypes )
{
  declareInterface<IRadiatorTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RadiatorTool::~RadiatorTool() {}
//=============================================================================

//=============================================================================
// initialize
//=============================================================================
StatusCode Rich::RadiatorTool::initialize ( )
{
  // intialise base class
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // UMS dependencies
  updMgrSvc()->registerCondition( this, DeRichLocations::Rich1Gas, 
                                  &Rich::RadiatorTool::radiatorUpdate );
  updMgrSvc()->registerCondition( this, DeRichLocations::Rich2Gas, 
                                  &Rich::RadiatorTool::radiatorUpdate );
  updMgrSvc()->registerCondition( this, DeRichLocations::Aerogel, 
                                  &Rich::RadiatorTool::radiatorUpdate );
  
  // run first update
  sc = sc && radiatorUpdate();

  return sc;
}

//=============================================================================
// finalize
//=============================================================================
StatusCode Rich::RadiatorTool::finalize()
{
  updMgrSvc()->unregister(this);
  return Rich::ToolBase::finalize();
}

//=============================================================================
// UMS update
//=============================================================================
StatusCode Rich::RadiatorTool::radiatorUpdate()
{

  // RICH1 gas
  m_radiators[Rich::Rich1Gas].clear();
  m_radiators[Rich::Rich1Gas].push_back( getDet<DeRichRadiator>(DeRichLocations::Rich1Gas) );

  // RICH2 gas
  m_radiators[Rich::Rich2Gas].clear();
  m_radiators[Rich::Rich2Gas].push_back( getDet<DeRichRadiator>(DeRichLocations::Rich2Gas) );

  // aerogel
  m_radiators[Rich::Aerogel].clear();
  const DeRichMultiSolidRadiator * aerogel
    = getDet<DeRichMultiSolidRadiator>( DeRichLocations::Aerogel );
  m_radiators[Rich::Aerogel].reserve( aerogel->radiators().size() );
  for ( DeRichRadiator::Vector::const_iterator dRad = aerogel->radiators().begin();
        dRad != aerogel->radiators().end(); ++dRad )
  {
    m_radiators[Rich::Aerogel].push_back( *dRad );
  }
  // Sort by distance from beam line
  std::stable_sort( m_radiators[Rich::Aerogel].begin(),
                    m_radiators[Rich::Aerogel].end(),
                    SortByDistFromBeam() );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Radiator information update" << endmsg;
    debug() << " -> Using the following DeRichRadiators :" << endmsg;
    for ( unsigned int i=0; i<m_radiators.size(); ++i )
    {
      for ( unsigned int j=0; j<m_radiators[i].size(); ++j )
      {
        debug() << "  -> " << m_radiators[i][j]->name() << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// intersections
//=============================================================================
unsigned int
Rich::RadiatorTool::intersections( const Gaudi::XYZPoint& globalPoint,
                                   const Gaudi::XYZVector& globalVector,
                                   const Rich::RadiatorType radiator,
                                   Rich::RadIntersection::Vector & intersections ) const
{

  // tally for the number of intersections
  unsigned int totalIntersections( 0 );

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
