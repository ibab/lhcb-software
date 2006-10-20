
//-----------------------------------------------------------------------------
/** @file RichRadiatorTool.cpp
 *
 *  Implementation file for tool : RichRadiatorTool
 *
 *  CVS Log :-
 *  $Id: RichRadiatorTool.cpp,v 1.6 2006-10-20 13:20:29 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2006-03-01
 */
//-----------------------------------------------------------------------------

// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichRadiatorTool.h"

// Rich Det
#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRadiatorTool
//
// 2006-03-01 : Antonis Papanestis
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRadiatorTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRadiatorTool::RichRadiatorTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichToolBase ( type, name, parent )
{
  declareInterface<IRichRadiatorTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
RichRadiatorTool::~RichRadiatorTool() {}
//=============================================================================

//=========================================================================
// initialize
//=========================================================================
StatusCode RichRadiatorTool::initialize ( )
{

  // intialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RICH1 gas
  m_radiators[Rich::Rich1Gas].clear();
  m_radiators[Rich::Rich1Gas].push_back( getDet<DeRichRadiator>(DeRichRadiatorLocation::Rich1Gas) );

  // RICH2 gas
  m_radiators[Rich::Rich2Gas].clear();
  m_radiators[Rich::Rich2Gas].push_back( getDet<DeRichRadiator>(DeRichRadiatorLocation::Rich2Gas) );

  // Rich1 DetElem
  const DetectorElement * rich1 = getDet<DetectorElement>(DeRichLocation::Rich1);

  // aerogel
  const IDetectorElement::IDEContainer& detelemsR1 = rich1->childIDetectorElements();
  for ( IDetectorElement::IDEContainer::const_iterator det_it =  detelemsR1.begin();
        det_it != detelemsR1.end();
        ++det_it )
  {
    const std::string& detName = (*det_it)->name();
    if ( detName.find("AerogelT") != std::string::npos )
      m_radiators[Rich::Aerogel].push_back( getDet<DeRichRadiator>( detName ));
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Using the following DeRichRadiators :-" << endmsg;
    for ( unsigned int i=0; i<m_radiators.size(); ++i )
      for ( unsigned int j=0; j<m_radiators[i].size(); ++j )
        debug() << "  " << m_radiators[i][j]->name() << endmsg;
  }

  return sc;
}

//=========================================================================
//  intersections
//=========================================================================
unsigned int
RichRadiatorTool::intersections( const Gaudi::XYZPoint& globalPoint,
                                 const Gaudi::XYZVector& globalVector,
                                 const Rich::RadiatorType radiator,
                                 RichRadIntersection::Vector & intersections ) const
{

  // tally for the number of intersections
  unsigned int totalIntersections( 0 );

  // loop over all volumes for given radiator
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
      intersections.push_back( RichRadIntersection( entry, globalVector, 
                                                    exit,  globalVector, 
                                                    *iR ) );
      ++totalIntersections;
    }
  }

  // If more than one intersection sort into increasing z position
  if ( totalIntersections > 1 )
  {
    RichRadIntersection::Sorter::sortByZ( intersections );
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << radiator << " Intersections :";
    for ( RichRadIntersection::Vector::const_iterator iS = intersections.begin();
          iS != intersections.end(); ++iS )
    {
      debug() << " " << (*iS).entryPoint() << "->" << (*iS).exitPoint();
    }
    debug() << endreq;
  }

  // return the number of intersections
  return totalIntersections;
}

//=========================================================================
